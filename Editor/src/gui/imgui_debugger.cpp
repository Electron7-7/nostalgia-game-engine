#include "imgui_debugger.hpp"
#include "imgui_editor.hpp"
#include "core/uid.hpp"
#include "core/enum_prettifier.hpp"
#include "backends/opengl/gl_renderer_api.hpp"
#include "managers/render_manager.hpp"
#include "tools/stopwatch_log.hpp"
#include "events/event.hpp"
#include "managers/event_manager.hpp"
#include "rendering/renderer_api.hpp"
#include "settings/player.hpp"
#include "settings/engine.hpp"
#include "settings/graphics.hpp"
#include "math/containers.hpp"
#include "managers/manager.hpp"
#include "managers/input_manager.hpp"
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
#include "DearImGui/imgui.h"
#include "DearImGui/imgui_stdlib.h"
#ifdef DEBUGGING
#   include "managers/physics_manager.hpp"
#   include "theatre/parser/theatre_parser.hpp"
#endif

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <set>
#include <format>
#include <memory>

// Because DearImGui loves c-strings and I don't
#define _fmtBool(BOOL) std::format("{}", static_cast<bool>(BOOL)).data()

using namespace ImGui;

static ImGui_Debugger sImGuiDebugger;
ImGui_Debugger* g_pImGuiDebugger{&sImGuiDebugger};

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

Error ImGui_Debugger::Init()
{
    PRINT_PRETTY_FUNCTION;
    return OK;
}

void ImGui_Debugger::Shutdown()
{ PRINT_PRETTY_FUNCTION; }

void ImGui_Debugger::Input(InputEvent* event)
{
    if(event->IsJustPressed(Key::F5) or
        (event->IsJustPressed(Key::L) and event->IsModifierActive(Key::Mod_Control)))
    {
        sLastAttemptedTheatreFilePath = sTheatreFilePath;
        g_pTheatreManager->LoadTheatreFromFile(sTheatreFilePath);
        print_debug("LoadTheatreFromFile called from {}", __PRETTY_FUNCTION__);
    }
    else if(event->IsJustPressed(Key::F4) or
        (event->IsJustPressed(Key::L) and event->IsModifierActive(Key::Mod_Control | Key::Mod_Shift)))
        { IManager::ShutdownTheatre(); }
    else if(event->IsJustPressed(Key::F3))
        { sTheatreInspectorActive = !sTheatreInspectorActive; }
}

void ImGui_Debugger::OnTheatreEntered()
{}

void ImGui_Debugger::OnTheatreExited()
{}

void ImGui_Debugger::Update()
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
            // InputText("Demo File Output", &sDemoFileOut);
            // if(Button("Start Recording Demo"))
                // { g_pDemoController->Record(); }
            // if(Button("Stop Recording Demo"))
                // { g_pDemoController->StopRecording(); g_pDemoController->Save(sDemoFileOut); }
            // InputText("Demo File Input", &sDemoFileIn);
            // if(Button("Play Demo File"))
                // { g_pDemoController->Play(sDemoFileIn); }
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

StopwatchLog& ImGui_Debugger::StartStopwatch(const std::string& message)
{
    if(!sAutoStopwatchEnabled)
        { return StopwatchLog::Invalid; }
    return m_StartStopwatch(message);
}

bool ImGui_Debugger::StopStopwatch(StopwatchLog& stopwatch)
{
    if(!sAutoStopwatchEnabled)
        { return false; }
    return m_StopStopwatch(stopwatch);
}

StopwatchLog& ImGui_Debugger::m_StartStopwatch(const std::string& message)
{
#pragma message("TODO: Make this thread safe")
    StopwatchLog& stopwatch = sStopwatchLogs.emplace_back();
    stopwatch.Start(message);
    return stopwatch;
}

bool ImGui_Debugger::m_StopStopwatch(StopwatchLog& stopwatch)
{
    if(stopwatch == StopwatchLog::Invalid || !stopwatch.Running())
        { return false; }
    else if(std::find(sStopwatchLogs.begin(), sStopwatchLogs.end(), stopwatch) == sStopwatchLogs.end())
        { return false; }
    return stopwatch.Stop();
}


void ImGui_Debugger::StartTheatreTiming(bool loading)
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

void ImGui_Debugger::StopTheatreTiming(bool loading)
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
        SeparatorText("Labels");
            ushort i{0};
            for(auto label : __all_labels_for_debugging)
            {
                Checkbox(std::format("ANSI Codes##{}", i++).data(),
                    &label->enable_ansi_sequence);
                SetNextItemWidth(500.0f);
                SameLine();
                InputText(label->name(),
                    &label->label(),
                    ImGuiInputTextFlags_AutoSelectAll      |
                    ImGuiInputTextFlags_EnterReturnsTrue   |
                    ImGuiInputTextFlags_ElideLeft);
            }
        SeparatorText("General");
            Checkbox("Print Event Logs", &g_pEventManager->mDebugPrintEverySingleEventToTheConsole);
            Checkbox("Print Input Logs", &g_pInputManager->mDebugPrintEverySingleEventToTheConsole);
            Checkbox("Print Frame#", &gDebugPrintFrameNumbers);
            SameLine();
            Checkbox("Print Tick#", &gDebugPrintTickNumbers);
        SeparatorText("Theatre");
            Checkbox("Print Theatre Parser Log", &gPrintLoadedTheatreData);
        SeparatorText("Rendering");
            Checkbox("Draw Command", &gPrintDrawLogs);
        if(RendererAPI::GetAPI() == GraphicsAPI::OpenGL)
        {
            SeparatorText("OpenGL");
            Text("Severity Levels");
            Separator();
            // Checkbox("High", &g_EnableDebugMsgHigh); SameLine();
            // Checkbox("Medium", &g_EnableDebugMsgMedium); SameLine();
            // Checkbox("Low", &g_EnableDebugMsgLow); SameLine();
            // Checkbox("Notification", &g_EnableDebugMsgNotif);
        }
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
                MainWindow()->GetTitle(),
                MainWindow()->GetXPosition(),
                MainWindow()->GetYPosition(),
                MainWindow()->GetWidth(),
                MainWindow()->GetHeight());
            TreePop();
        }
        bool fullscreen{MainWindow()->IsFullscreen()};
        if(Checkbox("Fullscreen", &fullscreen))
        {
            MainWindow()->SetWindowMode((fullscreen)
                ? IWindow::WINDOW_MODE_WINDOWED
                : IWindow::WINDOW_MODE_FULLSCREEN);
        }

#       ifndef WAYLAND_DISPLAY
            vector<2,int,VectorMembers::XYZ> position{MainWindow()->GetPosition()};
            if(DragInt2("Position", &position.x(), &position.y()))
                { MainWindow()->SetPosition(position); }

            int scale[2] {
                static_cast<int>(MainWindow()->GetScale().width()),
                static_cast<int>(MainWindow()->GetScale().height()),
            };
            if(DragInt2("Size", scale))
                { MainWindow()->SetScale({(uint)scale[0], (uint)scale[1]}); } // Evil c-style cast
            NewLine();
#       else
            Text("Position: [%d, %d]",
                MainWindow()->GetXPosition(),
                MainWindow()->GetYPosition());
            Text("Size: [%d, %d]",
                MainWindow()->GetWidth(),
                MainWindow()->GetHeight());
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
    InputInt("Line", &gBreakOnLine, 0, 10, ImGuiInputTextFlags_AutoSelectAll);
    InputInt("Column", &gBreakOnColumn, 0, 10, ImGuiInputTextFlags_AutoSelectAll);
    PushItemWidth(0.0f);
    Separator();
#endif // DEBUGGING
    Checkbox("Debug Theatre File Load Printout", &gPrintLoadedTheatreData);

    InputText("Theatre File", &sTheatreFilePath);

    if(IManager::GetTheatreState() != ManagerEnums::NOT_IN_LEVEL)
        { BeginDisabled(); }
    if(Button("Load Theatre"))
    {
        sLastAttemptedTheatreFilePath = sTheatreFilePath;
        g_pTheatreManager->LoadTheatreFromFile(sTheatreFilePath);
        print_debug("LoadTheatreFromFile called from {}", __PRETTY_FUNCTION__);
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
    Text("UID  - %d", thing->uid()[]);
}

static void s_ResourceInfo(ID uid, const char* tree_name)
{
    static uint id{0};
    static std::string name{""};
    if(g_pTheatreManager->ThingExists(uid) && TreeNode(tree_name))
    {
        auto thing = g_pTheatreManager->GetThing(uid);
        s_NameAndUID(thing);
        TreePop();
    }
    else if(UID::try_GetDataValues(id, name) && TreeNode(tree_name))
    {
        Text("Name - %s", name.data());
        Text("UID  - %u", id);
        TreePop();
    }
}

void ImGui_Debugger::s_InspectTheatreWindow(bool* is_active)
{
    static Shared<Actor> sActor{nullptr};
    static int s_MaxPerRow{3};
    static bool sActivateOnReset{false};

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
                Shared<Actor> actor{g_pTheatreManager->GetThing<Actor>(uid)};
                ImU32 push_color{IM_COL32(100, 103, 48, 255)};
                ImGuiCol_ push_col{(std::dynamic_pointer_cast<light_t>(actor))
                    ? ImGuiCol_Button
                    : ImGuiCol_TextDisabled};
                PushStyleColor(push_col, push_color);
                const char* button_name{(actor->name().empty()) ? "N/A" : actor->c_name()};
                if(Button(button_name, {(GetWindowWidth() / s_MaxPerRow) - 5.0f, 0.0f}))
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
            Text("Type - %s", sActor->type().c_name());
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
                    Text("Motion: %s", EnumPrettifier::Get(collider->Motion(), EnumSet::PhysicsBodyMotion).data());
                    Text("Shape: %s", EnumPrettifier::Get(collider->Shape(), EnumSet::PhysicsBodyShape).data());
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
