#include "imgui_debugger.hpp"
#include "imgui_editor.hpp"
#include "fwd/managers.hpp"
#include "fwd/theatre.hpp"
#include "core/uid.hpp"
#include "backends/opengl/gl_renderer_api.hpp"
#include "managers/render_manager.hpp"
#include "physics/engine.hpp"
#include "settings/engine.hpp"
#include "settings/graphics.hpp"
#include "settings/player.hpp"
#include "tools/stopwatch_log.hpp"
#include "events/event.hpp"
#include "rendering/renderer_api.hpp"
#include "math/containers.hpp"
#include "managers/manager.hpp"
#include "managers/input_manager.hpp"
#include "managers/theatre_manager.hpp"
#include "filesystem/filesystem.hpp"
#include "things/actors/light.hpp"
#include "things/actors/camera.hpp"
#include "things/devices/viewport.hpp"
#include "things/devices/material.hpp"
#include "things/devices/mesh_instance.hpp"
#include "things/devices/collider.hpp"
#include "things/resources/resource.hpp"
#include "theatre/parser/theatre_data.hpp"
#include "application/application.hpp"
#include "application/window.hpp"
#include "DearImGui/imgui.h"
#include "DearImGui/imgui_stdlib.h"
#include <Jolt/Physics/Body/BodyCreationSettings.h>
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

void ImGui_Debugger::Init()
{ PRINT_PRETTY_FUNCTION; }

void ImGui_Debugger::Shutdown()
{ PRINT_PRETTY_FUNCTION; }

void ImGui_Debugger::Input(InputEvent* event)
{
    if(event->IsJustPressed(Key::F4) or
        (event->IsJustPressed(Key::L) and event->IsModifierActive(Key::Mod_Control | Key::Mod_Shift)))
        { IManager::ShutdownTheatre(); }
    else if(event->IsJustPressed(Key::F5) or
        (event->IsJustPressed(Key::L) and event->IsModifierActive(Key::Mod_Control)))
    {
        sLastAttemptedTheatreFilePath = sTheatreFilePath;
        g_pTheatreManager->LoadTheatreFromFile(sTheatreFilePath);
        print_debug("LoadTheatreFromFile called from {}", __PRETTY_FUNCTION__);
    }
    else if(event->IsJustPressed(Key::F3))
    {
        sTheatreInspectorActive = (IManager::GetTheatreState() == ManagerEnums::IN_LEVEL)
            ? !sTheatreInspectorActive
            : false;
    }
}

void ImGui_Debugger::TheatreEntered()
{}

void ImGui_Debugger::TheatreExited()
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
    if(CollapsingHeader("Message Labels"))
    {
        ushort i{0};
        for(auto label : __all_labels_for_debugging)
        {
            if(TreeNode(label->name()))
            {
                Checkbox(std::format("Use ANSI Codes##{}",i).data(), &label->enable_ansi_sequence);
                SetNextItemWidth((label->label().size() * 7) + 8);
                InputText(std::format("Label##{}",i).data(), &label->label(),
                    ImGuiInputTextFlags_AutoSelectAll |
                    ImGuiInputTextFlags_NoHorizontalScroll);
                TreePop();
            }
            ++i;
        }
    }
    if(CollapsingHeader("Messages"))
    {
        SeparatorText("General");
            Checkbox("Print Event Logs", &gPrintEventLogs);
            Checkbox("Print Input Logs", &gPrintInputLogs);
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
            const char* gl_debug_severities{"None\0Notification\0Low\0Medium\0High\0"};
            static int gl_debug_selection{0};
            if(Combo("Message Minimum Severity", &gl_debug_selection, gl_debug_severities))
                { gOpenGLMessageFilter = static_cast<DebugMessageSeverityFilter>(gl_debug_selection); }
        }
        SeparatorText("Jolt Physics");
            Checkbox("Contact Validate",  &gJoltDebugMessageAllow_ContactValidate);  SameLine();
            Checkbox("Contact Added",     &gJoltDebugMessageAllow_ContactAdded);     SameLine();
            Checkbox("Contact Persisted", &gJoltDebugMessageAllow_ContactPersisted); SameLine();
            Checkbox("Contact Removed",   &gJoltDebugMessageAllow_ContactRemoved);
            Checkbox("Body Activated",    &gJoltDebugMessageAllow_BodyActivated);    SameLine();
            Checkbox("Body Deactivated",  &gJoltDebugMessageAllow_BodyDeactivated);
    }
#endif // DEBUGGING
    if(CollapsingHeader("Rendering"))
    {
        Checkbox("Global Wireframe Mode", &Settings::Graphics::GlobalWireframe);
        static int sSelected{0};
        static const char* sSelectableNames{"Default\0Vertex Colors\0Vertex Normals\0Vertex UVs\0"};
        if(Combo("Shader Output", &sSelected, sSelectableNames))
            { gShaderDebugOutput = static_cast<ushort>(sSelected); }
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
    InputUInt("Line", &gBreakOnLine, 0, 10, ImGuiInputTextFlags_AutoSelectAll);
    InputUInt("Column", &gBreakOnColumn, 0, 10, ImGuiInputTextFlags_AutoSelectAll);
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

    bool not_in_level = (IManager::GetTheatreState() != ManagerEnums::IN_LEVEL);

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

struct thing_data_buffer
{
    thing_data_buffer() noexcept = default;
    thing_data_buffer(Shared<Thing> inThingPtr) noexcept:
        ptr{inThingPtr},
        id{inThingPtr->uid()[]},
        name{inThingPtr->name()},
        activate_on_change{false}
    {
        if(auto transform3d{DCast<Transform3D>(ptr)})
        {
            origin = transform3d->Origin();
            rotation = transform3d->Euler(true);
            scale = transform3d->Scale();
        }
        if(auto actor{DCast<Actor3D>(ptr)})
        {
            visible = actor->Visible();
            wireframe = actor->Wireframe();
            debug_mesh_instance = actor->DebugMeshInstance()[];
            if(auto camera3d{DCast<Camera3D>(ptr)})
            {
                is_camera_current = camera3d->Current();
                fov = camera3d->mFOV;
                view_near = camera3d->mViewCutoffNear;
                view_far = camera3d->mViewCutoffFar;
                env_type = camera3d->mEnvironment.mType;
            }
        }
        else if(auto device{DCast<Device>(ptr)})
        {
            if(auto mesh_instance{DCast<MeshInstance3D>(ptr)})
            {
                mesh = mesh_instance->MeshID()[];
                material = mesh_instance->MaterialID()[];
            }
            else if(auto material{DCast<Material>(ptr)})
            {
                diffuseTexture  = material->DiffuseTextureID()[];
                specularTexture = material->SpecularTextureID()[];
            }
            else if(auto collider{DCast<Collider>(ptr)})
            {
                motion = static_cast<int>(collider->Motion());
                shape  = static_cast<int>(collider->Shape());
                collider_material = collider->Material();
                activate_on_change = collider->SetActiveOnNextChange();
            }
        }
    }
    Shared<Thing> ptr{nullptr};
    uint id{};
    std::string name{};
    glm::vec3 origin{},
        rotation{},
        scale{};
    uint mesh{},
        material{},
        diffuseTexture{},
        specularTexture{},
        debug_mesh_instance{};
    int motion{static_cast<int>(MotionType::None)},
        shape{static_cast<int>(ShapeType::None)};
    float fov{75.0f},
        view_near{0.001f},
        view_far{1000.0f},
        density{02.0f},
        friction{0.0f};
    bool visible{true},
        wireframe{false},
        activate_on_change{false},
        is_camera_current{false};
    Environment::BackgroundType env_type{Environment::BackgroundType::BG_CLEAR_COLOR};
    ColliderMaterial collider_material{};
};

void ImGui_Debugger::s_InspectTheatreWindow(bool* is_active)
{
    static uint mNewChildUID{0};
    static uint mNewParentUID{0};
    static thing_data_buffer selected{};
    static int sMaxPerRow{3};
    static float thing_button_color[3]{},
        actor_button_color[3]    {0.063f, 0.392f, 0.6f},
        light_button_color[3]    {0.494f, 0.494f, 0.494f},
        device_button_color[3]   {0.447f, 0.125f, 0.361f},
        viewport_button_color[3] {0.500f, 0.300f, 0.800f},
        resource_button_color[3] {0.608f, 0.204f, 0.165f},
        camera_button_color[3]   {0.808f, 0.707f, 0.086f};
    static bool show_type_on_button{true};
    if(IManager::GetTheatreState() != ManagerEnums::IN_LEVEL)
        { selected = thing_data_buffer{}; return; }
    else if(sTheatreInspectorActive)
    {
        if(Begin("Theatre Inspector", is_active))
        {
            if(CollapsingHeader("Selector Settings"))
            {
                InputInt("Max Buttons Per Row", &sMaxPerRow, 1, 5);
                ColorEdit3("Thing Color",    thing_button_color,    ImGuiColorEditFlags_Float);
                ColorEdit3("Actor3D Color",  actor_button_color,    ImGuiColorEditFlags_Float);
                ColorEdit3("Light Color",    light_button_color,    ImGuiColorEditFlags_Float);
                ColorEdit3("Device Color",   device_button_color,   ImGuiColorEditFlags_Float);
                ColorEdit3("Viewport Color", viewport_button_color, ImGuiColorEditFlags_Float);
                ColorEdit3("Resource Color", resource_button_color, ImGuiColorEditFlags_Float);
                ColorEdit3("Camera Color",   camera_button_color, ImGuiColorEditFlags_Float);
                Checkbox("Show Type Abbreviation", &show_type_on_button);
            }
            if(CollapsingHeader("Thing Selection", ImGuiTreeNodeFlags_DefaultOpen))
            {
                BeginChild("Buttons", {0,0}, ImGuiChildFlags_AutoResizeY);
                auto uids{g_pTheatreManager->GetThingIDs()};
                int item_counter{0}, name_counter{0};
                for(ID uid : uids)
                {
                    auto thing{g_pTheatreManager->GetThing(uid)};
                    auto actor{DCast<Actor3D>(thing)};
                    ImVec4 push_color{thing_button_color[0],thing_button_color[1],thing_button_color[2],1.0f};
                    ImGuiCol push_color_where{ImGuiCol_Button};
                    std::string type_symbol{"(T) "};
                    if(actor)
                    {
                        if(DCast<light_t>(thing))
                        {
                            push_color = {light_button_color[0],light_button_color[1],light_button_color[2],1.0f};
                            type_symbol = "(L) ";
                        }
                        else if(DCast<Camera3D>(thing))
                        {
                            push_color = {camera_button_color[0],camera_button_color[1],camera_button_color[2],1.0f};
                            type_symbol = "(C) ";
                        }
                        else
                        {
                            push_color = {actor_button_color[0],actor_button_color[1],actor_button_color[2],1.0f};
                            type_symbol = "(A) ";
                        }
                    }
                    else if(DCast<Device>(thing))
                    {
                        if(DCast<Viewport>(thing))
                        {
                            push_color = {viewport_button_color[0],viewport_button_color[1],viewport_button_color[2],1.0f};
                            type_symbol = "(V) ";
                        }
                        else
                        {
                            push_color = {device_button_color[0],device_button_color[1],device_button_color[2],1.0f};
                            type_symbol = "(D) ";
                        }
                    }
                    else if(DCast<Resource>(thing))
                    {
                        push_color = {resource_button_color[0],resource_button_color[1],resource_button_color[2],1.0f};
                        type_symbol = "(R) ";
                    }
                    else { push_color_where = ImGuiCol_TextDisabled; } // Basically "don't use"
                    std::string button_name{std::format("{}{}",
                        (show_type_on_button) ? type_symbol : "",
                        (thing->name().empty()) ? "N/A" : thing->name())};
                    PushStyleColor(push_color_where, push_color);
                    PushID(++name_counter);
                    if(Button(button_name.data(), {(GetWindowWidth() / sMaxPerRow) - 5.0f, 0.0f}))
                        { selected = thing_data_buffer{thing}; mNewChildUID = 0; }
                    PopID();
                    if(actor)
                        { actor->mDebugHighlight.a = IsItemHovered(); }
                    PopStyleColor();
                    if(++item_counter < sMaxPerRow) { SameLine(); }
                    else { item_counter = 0; }
                }
                EndChild();
            }
            if(!selected.ptr)
                { End(); return; }
            BeginChild("View Thing", {0,0}, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border);
            // THINGS
            if(Button(std::format("Destroy {}", selected.ptr->name()).data())
                and g_pTheatreManager->DestroyThing(selected.ptr->uid()))
                { EndChild(); End(); selected = {}; return; }
            SeparatorText("Properties");
            if(UID::IsReserved(selected.id))
            {
                TextF("UID: {}", selected.id);
                TextF("Name: {}", selected.name);
            }
            else
            {
                static Error id_change_status{FAILED};
                InputUInt("UID", &selected.id, 1, 5);
                if(IsItemDeactivatedAfterEdit())
                {
                    id_change_status = g_pTheatreManager->ChangeThingID(selected.ptr->uid(), selected.id);
                    if(!id_change_status)
                        { OpenPopup("Failed to set UID!"); }
                    SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                        ImGuiCond_Appearing,
                        {0.5f, 0.5f});
                }
                if(BeginPopupModal("Failed to set UID!", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    std::string reason{"N/A"};
                    if(id_change_status == ERR_NOT_FOUND)
                        { reason = std::format("No Actor3D found with UID#{}", selected.ptr->uid()[]); }
                    else if(id_change_status == ERR_ALREADY_EXISTS)
                    {
                        if(UID::IsReserved(selected.id))
                            { reason = std::format("UID#{} is a reserved UID", selected.id); }
                        else
                            { reason = std::format("UID#{} is already in use", selected.id); }
                    }
                    Text("%s", reason.data());
                    ImVec2 button_size{120, 0};
                    SetCursorPosX((ImGui::GetWindowSize().x - button_size.x) / 2);
                    if(Button("Ok##1", button_size))
                        { CloseCurrentPopup(); }
                    EndPopup();
                }
                if(InputText("Name", &selected.name))
                    { selected.ptr->name(selected.name); }
            }
            if(selected.ptr)
                { Text("Type - %s", selected.ptr->type().c_name()); }
            static bool collider_update_transform_instantly{false};
            // TRANSFORM 3D
            if(auto transform3d{DCast<Transform3D>(selected.ptr)})
            {
                bool (*ItemEdited_f)(){(DCast<Collider>(selected.ptr) and !collider_update_transform_instantly)
                    ? &IsItemDeactivatedAfterEdit
                    : &IsItemEdited};
                DragGLMv3("Position", &selected.origin, 0.05f, -200.0f, 200.0f, "%.2f");
                if(ItemEdited_f())
                    { transform3d->Origin(selected.origin); }
                DragGLMv3("Rotation", &selected.rotation, 0.1f, -359.995f, 359.995f, "%.2f", ImGuiSliderFlags_WrapAround);
                if(ItemEdited_f())
                    { transform3d->Euler(selected.rotation, true); }
                DragGLMv3("Scale", &selected.scale, 0.01f, -100.0f, 100.0f, "%.2f");
                if(ItemEdited_f())
                    { transform3d->Scale(selected.scale); }
            }
            // ACTORS
            if(auto actor{DCast<Actor3D>(selected.ptr)})
            {
                if(auto light{DCast<light_t>(actor)})
                {
                    Checkbox("Enabled", &light->mEnabled);
                    ColorEditGLMv3("Color", &light->mColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB);
                    InputFloat("Energy", &light->mEnergy, 0.01f, 0.05f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                    InputFloat("Specular Strength", &light->mSpecularStrength, 0.1f, 0.5f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                    InputFloat("Ambient Strength", &light->mAmbientStrength, 0.01f, 0.05f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                    InputFloat("Attenuation Scalar", &light->mAttenuation, 0.01f, 0.05f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                    InputFloat("Range", &light->mRange, 1.0f, 0.5f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                    if(DCast<SpotLight>(light))
                    {
                        InputFloat("SpotAngle", &light->mSpotAngle, 0.1f, 0.5f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                        InputFloat("SpotAngleFade", &light->mSpotAngleFade, 0.5f, 1.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                    }
                }
                if(Checkbox("Visible", &selected.visible))
                    { actor->Visible(selected.visible); }
                SameLine();
                if(Checkbox("Wireframe", &selected.wireframe))
                    { actor->Wireframe(selected.wireframe); }
                if(IsItemHovered())
                    { SetTooltip("%s", "Enabling the global wireframe setting will override this option"); }
                ColorEditGLMv4("DebugHighlight", &actor->mDebugHighlight, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoAlpha);
                if(auto camera3d{DCast<Camera3D>(selected.ptr)})
                {
                    auto cur_vp{g_pTheatreManager->GetThing(camera3d->ViewportID())};
                    auto viewports{g_pTheatreManager->GetViewportIDList()};
                    std::string current_viewport{std::format("{} [{}]",
                        cur_vp->name(),
                        cur_vp->uid()[])};
                    if(BeginCombo("Viewport", current_viewport.data()))
                    {
                        for(ID id : viewports)
                        {
                            const bool is_selected{cur_vp->uid() == id};
                            std::string label{std::format("{} [{}]",
                                g_pTheatreManager->GetThing(id)->name(),
                                id[])};
                            if(Selectable(label.data())
                                and camera3d->ViewportID(id) == OK)
                                { selected = thing_data_buffer{camera3d}; }
                            if(is_selected)
                                { SetItemDefaultFocus(); }
                        }
                        EndCombo();
                    }
                    if(Checkbox("Current", &selected.is_camera_current))
                    {
                        camera3d->Current(!camera3d->Current());
                        selected.is_camera_current = camera3d->Current();
                    }
                    if(SliderFloat("Vertical FOV", &selected.fov, 0.0f, 180.0f))
                        { camera3d->mFOV = selected.fov; }
                    if(DragFloat("View Cutoff Near", &selected.view_near, 0.001f, 0.001f, 100000.0f))
                        { camera3d->mViewCutoffNear = selected.view_near; }
                    if(DragFloat("View Cutoff Far", &selected.view_far, 1.0f, 0.001f, 100000.0f))
                        { camera3d->mViewCutoffFar = selected.view_far; }

                    static const std::map<Environment::BackgroundType, const char*>
                        bg_type{
                            { Environment::BG_SKYBOX, "SkyBox"},
                            { Environment::BG_CLEAR_COLOR, "Clear Color"},
                            { Environment::BG_CUSTOM_COLOR, "Custom Color"},
                        };
                    Environment::BackgroundType current_type{camera3d->mEnvironment.mType};
                    if(BeginCombo("Viewport", bg_type.at(current_type)))
                    {
                        for(FAUTO [bg, name] : bg_type)
                        {
                            const bool is_selected{bg == current_type};
                            if(Selectable(name))
                            {
                                camera3d->mEnvironment.mType = bg;
                                selected = thing_data_buffer{camera3d};
                            }
                            if(is_selected)
                                { SetItemDefaultFocus(); }
                        }
                        EndCombo();
                    }
                }
            }
            // DEVICES
            else if(auto device{DCast<Device>(selected.ptr)})
            {
                if(auto viewport{DCast<Viewport>(selected.ptr)})
                {
                    TextF("Size: {}", viewport->Size().data_log());
                    TextF("Framebuffer ID: {}", (viewport->Framebuffer()) ? std::to_string(viewport->Framebuffer()->ID()) : "nullptr");
                    SeparatorText("Cameras");
                    for(ID id : viewport->CameraIDs())
                    {
                        if(viewport->IsCurrentCamera(id))
                            { Text("\t%s (current)", g_pTheatreManager->GetThing(id)->c_name()); }
                        else
                        {
                            Text("\t%s", g_pTheatreManager->GetThing(id)->c_name());
                            SameLine();
                            if(Button("Make Current"))
                                { viewport->CurrentCamera(id); }
                        }
                    }
                }
                else if(auto collider{DCast<Collider>(selected.ptr)})
                {
                    if(CollapsingHeader("Jolt Properties"))
                    {
                        auto bodyid{collider->id()};
                        auto& interface{PhysicsEngine::I()->BodyInterface()};
                        TextF("BodyID (index#): {}", bodyid.GetIndex());
                        TextF("Position: [{}, {}, {}]",
                            interface.GetPosition(bodyid).GetX(),
                            interface.GetPosition(bodyid).GetY(),
                            interface.GetPosition(bodyid).GetZ());
                        TextF("Rotation (euler degrees): [{}, {}, {}]",
                            JPH::RadiansToDegrees(interface.GetRotation(bodyid).GetEulerAngles().GetX()),
                            JPH::RadiansToDegrees(interface.GetRotation(bodyid).GetEulerAngles().GetY()),
                            JPH::RadiansToDegrees(interface.GetRotation(bodyid).GetEulerAngles().GetZ()));
                        auto scale{collider->CreationSettings()->GetShape()->GetLocalBounds().GetSize()};
                        TextF("Scale: [{}, {}, {}]",
                            // collider->CreationSettings()->GetShape()->Draw(DebugRenderer *inRenderer, RMat44Arg inCenterOfMassTransform, Vec3Arg inScale, ColorArg inColor, bool inUseMaterialColors, bool inDrawWireframe));
                            scale.GetX(),
                            scale.GetY(),
                            scale.GetZ());
                    }
                    static const char* motion_names{"Static\0Dynamic\0Kinematic\0None\0"};
                    static const char* shape_names{"Box\0Sphere\0Capsule\0Cylinder\0None\0"};
                    if(InputFloat("Mass", &selected.density, 0.1f, 1.0f))
                        { collider->Mass(selected.density); }
                    if(InputFloat("Friction", &selected.collider_material.friction, 0.01f, 0.1f))
                    {
                        if(selected.collider_material.friction > 1.0f)
                            { selected.collider_material.friction = 1.0f; }
                        else if(selected.collider_material.friction < 0.0f)
                            { selected.collider_material.friction = 0.0f; }
                        collider->Material(selected.collider_material);
                    }
                    if(Combo("Motion", &selected.motion, motion_names))
                        { collider->Motion((MotionType)selected.motion, selected.activate_on_change); }
                    if(Combo("Shape", &selected.shape, shape_names))
                        { collider->Shape((ShapeType)selected.shape, selected.activate_on_change); }
                    if(Checkbox("Set Active on Change", &selected.activate_on_change))
                        { collider->SetActiveOnNextChange(selected.activate_on_change); selected = {collider}; }
                    Checkbox("Update Transform Values Instantly", &collider_update_transform_instantly);
                    TextF("Active: {}", collider->Active());
                    if(!collider->Active() and Button("Activate"))
                        { collider->Active(true); }
                    else if(collider->Active() and Button("Deactivate"))
                        { collider->Active(false); }
                }
                else if(auto mesh_instance{DCast<MeshInstance3D>(selected.ptr)})
                {
                    if(InputUInt("Mesh UID", &selected.mesh, 0, 0))
                        { mesh_instance->MeshID(selected.mesh); }
                    if(InputUInt("Material UID", &selected.material, 0, 0))
                        { mesh_instance->MaterialID(selected.material); }
                }
                else if(auto material{DCast<Material>(selected.ptr)})
                {
                    if(InputUInt("Diffuse Texture UID", &selected.diffuseTexture, 0, 0))
                        { material->DiffuseTextureID(selected.diffuseTexture); }
                    if(InputUInt("Specular Texture UID", &selected.specularTexture, 0, 0))
                        { material->SpecularTextureID(selected.specularTexture); }

                    Checkbox("Don't Use Textures", &material->mDontUseTexture);
                    Checkbox("Ignore Lighting", &material->mFullBright);
                    ColorEditGLMv3("Diffuse Color",
                        &material->mColor,
                        ImGuiColorEditFlags_Float |
                            ImGuiColorEditFlags_DisplayRGB |
                            ImGuiColorEditFlags_InputRGB);
                    InputInt("Specular Sharpness", &material->mSpecularSharpness, 2, 8);
                    InputFloat("Specular Strength", &material->mSpecularStrength, 0.05f, 0.1f, "%.3f");
                }
            }
            // PARENTS
            BeginChild("Parents", {}, ImGuiChildFlags_AutoResizeY);
                SeparatorText("Parents");
                int parent_counter{0};
                if(Button("+##1"))
                {
                    selected.ptr->add_parent({mNewParentUID, g_pTheatreManager->GetThing(mNewParentUID)->type()}, true);
                    selected = {selected.ptr};
                    mNewParentUID = 0;
                    EndChild(); End();
                    return;
                }
                SameLine();
                InputUInt("UID##3", &mNewParentUID, 0, 0);
                for(FAUTO parent : selected.ptr->parents())
                {
                    PushID(++parent_counter);
                    if(Button("-"))
                        { selected.ptr->remove_parent(parent, true); PopID(); EndChild(); EndChild(); End(); return; }
                    SameLine();
                    if(auto parent_ptr{g_pTheatreManager->GetThing(parent.id)};
                        !parent.id.invalid() and !parent_ptr->uid().invalid())
                    {
                        TextF("Parent: <{}> {} [{}]",
                            parent_ptr->type().name(),
                            parent_ptr->name(),
                            parent_ptr->uid()[]);
                        SameLine();
                        if(Button("Inspect"))
                            { selected = thing_data_buffer{parent_ptr}; }
                    }
                    else
                        { TextF("Invalid Parent"); SameLine(); }
                    PopID();
                }
            EndChild();
            // CHILDREN
            BeginChild("Children", {}, ImGuiChildFlags_AutoResizeY);
                SeparatorText("Children");
                if(auto actor{DCast<Actor3D>(selected.ptr)})
                {
                    if(InputUInt("Debug MeshInstance3D UID", &selected.debug_mesh_instance, 0, 0))
                        { actor->DebugMeshInstance(selected.debug_mesh_instance); }
                }
                auto children{selected.ptr->children()};
                if(Button("+##2"))
                {
                    selected.ptr->add_child({mNewChildUID, g_pTheatreManager->GetThing(mNewChildUID)->type()}, true);
                    selected = {selected.ptr};
                    mNewChildUID = 0;
                    EndChild(); End();
                    return;
                }
                SameLine();
                InputUInt("UID##2", &mNewChildUID, 0, 0);
                uint child_counter{0};
                for(FAUTO child : children)
                {
                    auto thing{g_pTheatreManager->GetThing(child.id)};
                    if(thing->uid().invalid())
                        { TextF("[Invalid Child UID]##{}", ++child_counter); }
                    else
                    {
                        PushID(++child_counter + thing->uid()[]);
                        if(Button("-"))
                            { selected.ptr->remove_child(child, true); selected = {selected.ptr}; PopID(); EndChild(); EndChild(); End(); return; }
                        SameLine();
                        TextF("<{}> {} [{}]",
                            thing->type().name(),
                            thing->name(),
                            thing->uid()[]);
                        SameLine();
                        if(Button("Inspect"))
                            { selected = thing_data_buffer{thing}; }
                        PopID();
                    }
                }
            EndChild();
            EndChild();
        }
        End();
    }
}
