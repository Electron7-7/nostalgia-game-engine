#include "imgui_debugger.hpp"
#include "main_window.hpp"
#include "tools/stopwatch_log.hpp"
#include "settings/player.hpp"
#include "settings/engine.hpp"
#include "settings/graphics.hpp"
#include "input/demo_controller.hpp"
#include "managers/manager.hpp"
#include "managers/theatre_manager.hpp"
#include "filesystem/filesystem.hpp"
#include "things/thing_factory.hpp"
#include "things/actors/light.hpp"
#include "things/devices/mesh_instance.hpp"
#include "things/devices/material.hpp"
#include "things/devices/collider.hpp"
#include "theatre/parser/theatre_data.hpp"
#include "application/application.hpp"
#include "application/window.hpp"
#ifdef DEBUGGING
#   include "managers/physics_manager.hpp"
#   include "theatre/parser/theatre_parser.hpp"
#endif // DEBUGGING

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <set>
#include <format>
#include <memory>

// Because DearImGui loves c-strings and I don't
#define _fmtBool(BOOL) std::format("{}", static_cast<bool>(BOOL)).data()

using namespace ImGui;

static bool sTheatreInspectorActive{false};
static bool sAutoStopwatchEnabled{true};

static std::string               sTheatreFilePath{"theatres/HelloWorld.nt"};
static std::string               sLastAttemptedTheatreFilePath{sTheatreFilePath};
static std::string               sDemoFileOut{"demo"};
static std::string               sDemoFileIn{std::format("{}.dem", sDemoFileOut)};
static std::vector<TheatreLog>   sTheatreLogs{};
static std::vector<StopwatchLog> sManualStopwatchLogs{};
static std::vector<StopwatchLog> sStopwatchLogs{};
static std::set<int>             sStopwatchLogIds{};

static void s_AutomaticStopwatchWindow(float);
static void s_ManualStopwatchWindow(float);
static void s_TheatreDebuggingWindow();
static void s_GeneralDebuggingWindow();

static imgui_Debugger sDebugger;
imgui_Debugger* g_pDebugger = &sDebugger;

void imgui_Debugger::Update()
{
    static bool sPopOutStopwatches{false};
    if(sTheatreInspectorActive)
        { s_InspectTheatreWindow(&sTheatreInspectorActive); }
    SetNextWindowSize({840,530}, ImGuiCond_FirstUseEver);
    if(gShowDebugWindow)
    {
        if(Begin("Debugging", &gShowDebugWindow, ImGuiWindowFlags_MenuBar))
        {
            if(BeginTabBar("Debug Tools"))
            {
                if(BeginTabItem("General"))
                {
                    s_GeneralDebuggingWindow();
                    EndTabItem();
                }
                if(sPopOutStopwatches)
                {
                    SetNextWindowSize({780,458}, ImGuiCond_FirstUseEver);
                    if(Begin("Stopwatches"))
                    {
                        float width = (GetWindowWidth() / 2.0f) - 12.0f;
                        s_AutomaticStopwatchWindow(width);
                        SameLine();
                        s_ManualStopwatchWindow(width);
                    }
                    End();
                }
                if(BeginTabItem("Stopwatches"))
                {
                    const char* pop_out_label = (sPopOutStopwatches) ? "Bring Back Window" : "Pop Out Window";
                    if(Button(pop_out_label))
                        { sPopOutStopwatches = !sPopOutStopwatches; }
                    if(!sPopOutStopwatches)
                    {
                        float width = (GetWindowWidth() / 2.0f) - 12.0f;
                        s_AutomaticStopwatchWindow(width);
                        SameLine();
                        s_ManualStopwatchWindow(width);
                    }
                    EndTabItem();
                }
                if(BeginTabItem("Theatres"))
                {
                    s_TheatreDebuggingWindow();
                    EndTabItem();
                }
            }
            EndTabBar();
            InputText("Demo File Output", &sDemoFileOut);
            if(Button("Start Recording Demo"))
                { g_pDemoController->Record(); }
            if(Button("Stop Recording Demo"))
                { g_pDemoController->StopRecording(); g_pDemoController->Save(sDemoFileOut); }
            InputText("Demo File Input", &sDemoFileIn);
            if(Button("Play Demo File"))
                { g_pDemoController->Play(sDemoFileIn); }
        }
        End();
    }
}

void s_HandleAutomaticStopwatchToggle()
{
    sAutoStopwatchEnabled = !sAutoStopwatchEnabled;
    if(sAutoStopwatchEnabled)
        { return; }
    for(StopwatchLog& log : sStopwatchLogs)
    {
        if(log.Stop())
            { log.OverrideMessage("Interrupted by disabling automatic stopwatches"); }
    }
}

StopwatchLog& imgui_Debugger::StartStopwatch(const std::string& message)
{
    if(!sAutoStopwatchEnabled)
        { return StopwatchLog::Invalid; }
    return m_StartStopwatch(message);
}

bool imgui_Debugger::StopStopwatch(StopwatchLog& stopwatch)
{
    if(!sAutoStopwatchEnabled)
        { return false; }
    return m_StopStopwatch(stopwatch);
}

StopwatchLog& imgui_Debugger::m_StartStopwatch(const std::string& message)
{
    #pragma message("TODO: Make this thread safe")
    StopwatchLog& stopwatch = sStopwatchLogs.emplace_back();
    stopwatch.Start(message);
    return stopwatch;
}

bool imgui_Debugger::m_StopStopwatch(StopwatchLog& stopwatch)
{
    if(stopwatch == StopwatchLog::Invalid || !stopwatch.Running())
        { return false; }
    else if(std::find(sStopwatchLogs.begin(), sStopwatchLogs.end(), stopwatch) == sStopwatchLogs.end())
        { return false; }
    return stopwatch.Stop();
}


void imgui_Debugger::StartTheatreTiming(bool loading)
{
    if(loading)
    {
        TheatreLog& log = sTheatreLogs.emplace_back();
        log.load_time.Start();
        log.name_or_file_path = sLastAttemptedTheatreFilePath;
    }
    else
    {
        if(sTheatreLogs.empty())
            { return; }
        TheatreLog& log = sTheatreLogs.back();
        log.unload_time.Start();
        if(log.load_time.Running())
            { log.load_time.Stop(); }
    }
}

void imgui_Debugger::StopTheatreTiming(bool loading)
{
    if(sTheatreLogs.empty())
        { return; }
    TheatreLog& log = sTheatreLogs.back();
    if(loading)
        { log.load_time.Stop(); }
    else
        { log.unload_time.Stop(); }
}

static void s_GeneralDebuggingWindow()
{
    BeginChild("General Debugging");
#ifdef DEBUGGING
    if(CollapsingHeader("Messages"))
    {
        SeparatorText("General");
            Checkbox("Print Frame#", &g_PrintFrameNumbers);
            SameLine();
            Checkbox("Print Tick#", &g_PrintTickNumbers);
        /*if(Settings::Engine::GraphicsBackend == BackendIDs::gOpenGL)
        {
            SeparatorText("OpenGL");
            Text("Severity Levels");
            Separator();
            Checkbox("High", &g_EnableDebugMsgHigh); SameLine();
            Checkbox("Medium", &g_EnableDebugMsgMedium); SameLine();
            Checkbox("Low", &g_EnableDebugMsgLow); SameLine();
            Checkbox("Notification", &g_EnableDebugMsgNotif);
        }*/
        SeparatorText("Jolt Physics");
        SeparatorText("Contact");
        Checkbox("Validate", &gEnableMsg_ContactValidate); SameLine();
        Checkbox("Added", &gEnableMsg_ContactAdded); SameLine();
        Checkbox("Persisted", &gEnableMsg_ContactPersisted); SameLine();
        Checkbox("Removed", &gEnableMsg_ContactRemoved);
        SeparatorText("Body");
        Checkbox("Activated", &gEnableMsg_BodyActivated); SameLine();
        Checkbox("Deactivated", &gEnableMsg_BodyDeactivated);
    }
#endif // DEBUGGING
    if(CollapsingHeader("Rendering"))
    {
        Checkbox("Global Wireframe Mode", &Settings::Graphics::GlobalWireframe);
        /*Text("Shader Output");
        Separator();
        static int s_Selected = Shader_ALL;
        static const char* s_SelectableNames[4] =
        {
            "Default",
            "Vertex Colors",
            "Vertex Normals",
            "Vertex UVs"
        };
        for(int i = 0 ; i < 4 ; ++i)
        {
            bool l_Selected = (s_Selected == i);
            if(Checkbox(s_SelectableNames[i], &l_Selected))
                { s_Selected = i; }
        }
        g_ShaderDebugOuptut = s_Selected;*/
    }
    if(CollapsingHeader("Engine"))
    {
        DragInt("Tick Rate", &Settings::Engine::TickRate);
        Text("Tick Interval: %f", Settings::Engine::TickInterval());
    }
    if(CollapsingHeader("Window"))
    {
        if(TreeNode("Window Info"))
        {
            Text("Title: %s\nPosition: [%d, %d]Size: [%d, %d]",
                g_pApplication->GetWindow().GetTitle(),
                g_pApplication->GetWindow().GetXPosition(),
                g_pApplication->GetWindow().GetYPosition(),
                g_pApplication->GetWindow().GetWidth(),
                g_pApplication->GetWindow().GetHeight());
            TreePop();
        }
        bool fullscreen{g_pApplication->GetWindow().IsFullscreen()};
        if(Checkbox("Fullscreen", &fullscreen))
        {
            g_pApplication->GetWindow().SetWindowMode((fullscreen)
                ? IWindow::WINDOW_MODE_WINDOWED
                : IWindow::WINDOW_MODE_FULLSCREEN);
        }

#       ifndef WAYLAND_DISPLAY
            auto position{g_pApplication->GetWindow().GetPosition()};
            if(DragInt2("Position", &position.x, &position.y))
                { g_pApplication->GetWindow().SetPosition(position); }

            int scale[2] {
                (int)g_pApplication->GetWindow().GetScale().width, // Evil c-style cast
                (int)g_pApplication->GetWindow().GetScale().height, // Evil c-style cast
            };
            if(DragInt2("Size", scale))
                { g_pApplication->GetWindow().SetScale({(uint)scale[0], (uint)scale[1]}); } // Evil c-style cast
            NewLine();
#       else
            Text("Position: [%d, %d]",
                g_pApplication->GetWindow().GetXPosition(),
                g_pApplication->GetWindow().GetYPosition());
            Text("Size: [%d, %d]",
                g_pApplication->GetWindow().GetWidth(),
                g_pApplication->GetWindow().GetHeight());
#       endif // WAYLAND_DISPLAY
    }
    if(CollapsingHeader("Player"))
    {
        SeparatorText("Camera");
        SliderFloat("Vertical FOV", &Settings::Player::FOV, 0.0f, 180.0f);
        DragFloat("View Cutoff Near", &Settings::Player::ViewCutoffNear, 0.001f, 0.001f, 100000.0f);
        DragFloat("View Cutoff Far", &Settings::Player::ViewCutoffFar, 1.0f, 0.001f, 100000.0f);
        SeparatorText("Movement");
        SliderFloat("Movement Speed", &Settings::Player::MovementSpeed, 0.0f, 10.0f);
        SeparatorText("Mouse");
#pragma message("TODO: re-implement raw mouse motion")
        if(Checkbox("Raw Mouse Motion", &Settings::Player::RawMouseMotion))
            { print_debug("Raw mouse motion toggle not yet implemented"); }
        SliderFloat("Mouse Sensitivity", &Settings::Player::MouseSensitivity, 0.0f, 5.0f);
        SliderFloat("Mouse Sensitivity Multiplier", &Settings::Player::MouseSensitivityScale, 0.0f, 1.0f);
    }
    EndChild();
}

static void s_PrintStopwatchLog(const StopwatchLog& stopwatch, size_t i)
{
    if(!stopwatch.Finished())
        { return; }
    Separator();
    Text("Stopwatch #%zu", i);
    Text("Time: %f", stopwatch.Duration());
    if(IsItemHovered())
        { SetTooltip("Started @ %s (hi-res: %f)\nStopped @ %s (hi-res: %f)", stopwatch.StartTime().c_str(), stopwatch.HiResStartTime(), stopwatch.StopTime().c_str(), stopwatch.HiResStopTime()); }
    Text("Message: '%s'", stopwatch.Message().c_str());
}

static void s_AutomaticStopwatchWindow(float width)
{
    BeginChild("Automatic Stopwatch", {width, 0}, ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_Borders);
    Text("%s", "Automatic Stopwatch");
    static bool s_DummyAutoStopwatchEnabled = true;
    Checkbox("Enable Automatic Stopwatch", &s_DummyAutoStopwatchEnabled);
    if(IsItemHovered())
        { SetTooltip("%s", "This enables/disables the program's ability to create new stopwatches"); }
    if(s_DummyAutoStopwatchEnabled != sAutoStopwatchEnabled)
        { s_HandleAutomaticStopwatchToggle(); }

    if(Button("Clear Logs"))
        { sStopwatchLogs.clear(); }

    BeginGroup();
        static bool sort_newest_first = false;
        ImGuiDir button_arrow_direction = (sort_newest_first) ? ImGuiDir_Down : ImGuiDir_Up;

        Text("Sorting by: %s", (sort_newest_first) ? "Newest" : "Oldest");
        SameLine();
        if(ArrowButton("SortingDirection", button_arrow_direction))
            { sort_newest_first = !sort_newest_first; }
    EndGroup();

    Text("Logs:");
    if(!sStopwatchLogs.empty())
    {
        std::vector<StopwatchLog> logs = {sStopwatchLogs.cbegin(), sStopwatchLogs.cend()};

        if(sort_newest_first)
        {
            for(size_t i = (logs.size() - 1) ; i > 0 ; --i)
                { s_PrintStopwatchLog(logs.at(i), i); }
        }
        else
        {
            for(size_t i = 0 ; i < logs.size() ; ++i)
                { s_PrintStopwatchLog(logs.at(i), i); }
        }
    }
    EndChild();
}

static void s_ManualStopwatchWindow(float width)
{
    BeginChild("Manual Stopwatch", {width, 0}, ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_Borders);
    Text("%s", "Manual Stopwatch");
    if(Button("Start Stopwatch"))
    {
        if(sManualStopwatchLogs.empty() || !sManualStopwatchLogs.back().Running())
            { sManualStopwatchLogs.emplace_back().Start(); }
    }
    if(Button("Stop Stopwatch"))
    {
        if(!sManualStopwatchLogs.empty() && sManualStopwatchLogs.back().Running())
            { sManualStopwatchLogs.back().Stop(); }
    }

    std::string last_stopwatch_time = (sManualStopwatchLogs.empty()) ? "N/A" : std::to_string(sManualStopwatchLogs.back().Duration());
    Text("Most Recent Stopwatch: %s", last_stopwatch_time.c_str());

    if(Button("Clear Logs"))
        { sManualStopwatchLogs.clear(); }
    Text("Logs:");

    if(!sManualStopwatchLogs.empty())
    {
        Separator();
        for(size_t i = 0 ; i < sManualStopwatchLogs.size() ; ++i)
        {
            const StopwatchLog& stopwatch = sManualStopwatchLogs.at(i);
            Text("Manual Stopwatch #%zu", i);
            Text("Time: %f", stopwatch.Duration());
            if(IsItemHovered())
            {
                std::string tooltip_extra = (stopwatch.Finished()) ? std::format("{}", stopwatch.StopTime()) : "N/A";
                SetTooltip("Started @ %s\nStopped @ %s", stopwatch.StartTime().c_str(), tooltip_extra.c_str());
            }
        }
    }
    EndChild();
}

static void s_TheatreDebuggingWindow()
{
    BeginChild("Theatre Debugging");
#ifdef DEBUGGING
    Text("Theatre File Parser Breakpoint:");
    PushItemWidth(82.0f);
    InputInt("Line", &g_BreakOnLine, 0, 10, ImGuiInputTextFlags_AutoSelectAll);
    InputInt("Column", &g_BreakOnColumn, 0, 10, ImGuiInputTextFlags_AutoSelectAll);
    PushItemWidth(0.0f);
    Separator();
#endif // DEBUGGING

    InputText("Theatre File", &sTheatreFilePath);

    if(IManager::GetTheatreState() != ManagerEnums::NOT_IN_LEVEL)
        { BeginDisabled(); }
    if(Button("Load Theatre"))
    {
        sLastAttemptedTheatreFilePath = sTheatreFilePath;
        g_pTheatreManager->LoadTheatreFromFile(sTheatreFilePath);
    }
    if(IManager::GetTheatreState() != ManagerEnums::NOT_IN_LEVEL)
        { EndDisabled(); }

    static std::string l_TheatreFilePath{"SavedTheatre.nt"};
    if(IManager::GetTheatreState() != ManagerEnums::IN_LEVEL)
        { BeginDisabled(); }
    if(Button("Save Theatre State"))
        { FileSystem::try_WriteFileFromString(l_TheatreFilePath, g_pTheatreManager->GetCurrentState().formatted()); }
    SameLine(); InputText("File Path", &l_TheatreFilePath, ImGuiInputTextFlags_CharsNoBlank);
    if(Button("Exit Theatre"))
        { IManager::ShutdownTheatre(); }
    if(IManager::GetTheatreState() != ManagerEnums::IN_LEVEL)
        { EndDisabled(); }

    bool not_in_level = (IManager::GetTheatreState() == ManagerEnums::NOT_IN_LEVEL);

    if(not_in_level)
    {
        BeginDisabled();
        sTheatreInspectorActive = false;
    }
    if(Button("Inspect Theatre"))
        { sTheatreInspectorActive = !sTheatreInspectorActive; }
    if(not_in_level)
        { EndDisabled(); }

    if(Button("Clear Logs"))
        { sTheatreLogs.clear(); }
    Text("Logs:");

    for(const TheatreLog& log : sTheatreLogs)
    {
        Separator();
        Text("Theatre File: %s", log.name_or_file_path.c_str());
        if(log.load_time.Finished())
        {
            Text("Load Time: %fs", log.load_time.Duration());
            if(IsItemHovered())
                { SetTooltip("Started @ %s\nStopped @ %s", log.load_time.StartTime().c_str(), log.load_time.StopTime().c_str()); }
        }
        if(log.unload_time.Finished())
        {
            Text("Unload Time: %fs", log.unload_time.Duration());
            if(IsItemHovered())
                { SetTooltip("Started @ %s\nStopped @ %s", log.unload_time.StartTime().c_str(), log.unload_time.StopTime().c_str()); }
        }
    }
    EndChild();
}

static void s_NameAndUID(const auto& thing)
{
    Text("Name - %s", thing->c_name());
    if(thing->uid() != ID::Invalid)
        { Text("UID  - %u", (id_t)thing->uid()); }
    else
        { Text("UID  - ID::Invalid"); }
}

static void s_ResourceInfo(ID uid, const char* tree_name)
{
    if(g_pTheatreManager->ThingExists(uid) && TreeNode(tree_name))
    {
        auto thing = g_pTheatreManager->GetThing(uid);
        s_NameAndUID(thing);
        TreePop();
    }
    else if(UniqueID::IsReserved(uid) && TreeNode(tree_name))
    {
        Text("Name - %s", UniqueID::Reserved::EmbeddedResourceNames.at(uid).data());
        Text("UID  - %u", static_cast<id_t>(uid));
        TreePop();
    }
}

void imgui_Debugger::s_InspectTheatreWindow(bool* is_active)
{
    static std::shared_ptr<Actor> sActor = nullptr;
    static int s_MaxPerRow = 3;
    static bool sActivateOnReset = false;
    if(Begin("Theatre Inspector", is_active))
    {
        if(CollapsingHeader("Thing Selection", ImGuiTreeNodeFlags_DefaultOpen))
        {
            PushItemFlag(ImGuiItemFlags_NoNavDefaultFocus, true);
            InputInt("Max Buttons Per Row", &s_MaxPerRow, 1, 5);
            PopItemFlag();
            BeginChild("Buttons", {0,0}, ImGuiChildFlags_AutoResizeY);
            std::vector<ID> ids{g_pTheatreManager->GetThingIDs()};
            int i{0};
            for(auto uid : ids)
            {
                if(!g_pThingFactory->IsDerivedFrom<Actor>(g_pTheatreManager->GetType(uid)))
                    { continue; }
                std::shared_ptr<Actor> actor{g_pTheatreManager->GetThing<Actor>(uid)};
                ImU32 push_color = IM_COL32(100, 103, 48, 255);
                ImGuiCol_ push_col = (std::dynamic_pointer_cast<light_t>(actor))
                    ? ImGuiCol_Button
                    : ImGuiCol_TextDisabled;
                PushStyleColor(push_col, push_color);
                if(Button(actor->c_name(), {(GetWindowWidth() / s_MaxPerRow) - 5.0f, 0.0f}))
                    { sActor = g_pTheatreManager->GetThing<Actor>(uid); sActivateOnReset = false; }
                actor->mDebugHighlight.a = IsItemHovered();
                PopStyleColor();
                if(++i < s_MaxPerRow) { SameLine(); }
                else { i = 0; }
            }
            EndChild();
        }

        if(sActor && IManager::GetTheatreState() == ManagerEnums::IN_LEVEL)
        {
            BeginChild("View Actor", {0,0}, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border);
            std::string destroy_text{std::format("Destroy {}", sActor->c_name())};
            if(Button(destroy_text.data()) && g_pTheatreManager->DestroyThing(sActor->uid()))
                { EndChild(); End(); sActor = nullptr; return; }

            SeparatorText("Immutable Properties");
            s_NameAndUID(sActor);
            Text("Type - %s", g_pThingFactory->GetTypeName(sActor->type()).data());
            Text("Quaternion - (%f, %f, %f, %f)", sActor->Quaternion().w, sActor->Quaternion().x, sActor->Quaternion().y, sActor->Quaternion().z);
            if(IsItemHovered())
                { SetTooltip("%s", "(w, x, y, z)"); }

            SeparatorText("Mutable Properties");
            if(auto light = std::dynamic_pointer_cast<light_t>(sActor))
            {
                Checkbox("Enabled", &light->mEnabled);
                ColorEditGLMv3("Color", &light->mColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB);
                InputFloat("Energy", &light->mEnergy, 0.01f, 0.05f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                InputFloat("Specular Strength", &light->mSpecularStrength, 0.1f, 0.5f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                InputFloat("Ambient Strength", &light->mAmbientStrength, 0.01f, 0.05f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                InputFloat("Attenuation Scalar", &light->mAttenuation, 0.01f, 0.05f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                InputFloat("Range", &light->mRange, 1.0f, 0.5f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                if(std::dynamic_pointer_cast<SpotLight>(light))
                {
                    InputFloat("SpotAngle", &light->mSpotAngle, 0.1f, 0.5f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                    InputFloat("SpotAngleFade", &light->mSpotAngleFade, 0.5f, 1.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                }
            }
            Checkbox("Visible", &sActor->mVisible); SameLine();
            Checkbox("Actor Wireframe", &sActor->mWireframe);
            if(IsItemHovered())
                { SetTooltip("%s", "Enabling the global wireframe setting will override this option"); }
            ColorEditGLMv4("DebugHighlight", &sActor->mDebugHighlight, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoAlpha);
            glm::vec3 Origin{sActor->Origin()};
            if(DragGLMv3("Position", &Origin, 0.05f, -200.0f, 200.0f, "%.2f"))
                { sActor->SetOrigin(Origin); }
            glm::vec3 Euler{sActor->Euler(true)};
            if(DragGLMv3("Rotation", &Euler, 0.1f, -359.995f, 359.995f, "%.2f", ImGuiSliderFlags_WrapAround))
                { sActor->SetEuler(Euler, true); }
            glm::vec3 Scale{sActor->Scale()};
            if(DragGLMv3("Scale", &Scale, 0.01f, -100.0f, 100.0f, "%.2f"))
                { sActor->SetScale(Scale); }

            NewLine();

            if(CollapsingHeader("Children", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if(g_pTheatreManager->ThingExists(sActor->MeshInstanceID()))
                {
                    SeparatorText("MeshInstance");
                    auto mesh_instance = g_pTheatreManager->GetThing<MeshInstance>(sActor->MeshInstanceID());
                    s_NameAndUID(mesh_instance);
                    s_ResourceInfo(mesh_instance->GetMeshID(), "Mesh");
                    if(g_pTheatreManager->ThingExists(mesh_instance->GetMaterialID()))
                    {
                        SeparatorText("Material");
                        auto material = g_pTheatreManager->GetThing<Material>(mesh_instance->GetMaterialID());
                        s_NameAndUID(material);
                        s_ResourceInfo(material->GetDiffuseTexture(),  "Diffuse Texture");
                        s_ResourceInfo(material->GetSpecularTexture(), "Specular Texture");
                        Checkbox("Don't Use Textures", &material->mDontUseTexture);
                        Checkbox("Ignore Lighting", &material->mFullBright);
                        ColorEditGLMv3("Diffuse Color", &material->mColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB);
                        InputInt("Specular Sharpness", &material->mSpecularSharpness, 2, 8);
                        InputFloat("Specular Strength", &material->mSpecularStrength, 0.05f, 0.1f, "%.3f");
                    }
                }
                if(g_pTheatreManager->ThingExists(sActor->ColliderID()))
                {
                    SeparatorText("Collider");
                    auto collider = g_pTheatreManager->GetThing<Collider>(sActor->ColliderID());
                    s_NameAndUID(collider);
                    Text("Motion: %s", collider->Motion().name());
                    Text("Shape: %s", collider->Shape().name());
                    bool active{collider->Active()};
                    Text("Is Active: %s", _fmtBool(active));
                    if(!active)
                    {
                        if(Button("Activate"))
                            { collider->Activate(); }
                    }
                    else if(active)
                    {
                        if(Button("Deactivate"))
                            { collider->Deactivate(); }
                    }
                    bool reset_pressed = Button("Reset Transform"); SameLine();
                    Checkbox("Activate on Reset", &sActivateOnReset);
                    if(reset_pressed)
                        { collider->ResetTransform(sActivateOnReset); }
                }
            }
            EndChild();
        }
        else
            { sActor = nullptr; }
    }
    End();
}
