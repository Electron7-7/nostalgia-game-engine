#include "imgui_debugger.hpp"
#include "imgui_editor.hpp"
#include "fwd/managers.hpp"
#include "fwd/theatre.hpp"
#include "core/uid.hpp"
#include "core/bitmask.hpp"
#include "backends/opengl/gl_renderer_api.hpp"
#include "managers/render_manager.hpp"
#include "physics/engine.hpp"
#include "settings/engine.hpp"
#include "settings/graphics.hpp"
#include "settings/player.hpp"
#include "theatre/things/resources/mesh.hpp"
#include "tools/stopwatch_log.hpp"
#include "events/event.hpp"
#include "rendering/renderer_api.hpp"
#include "math/containers.hpp"
#include "managers/manager.hpp"
#include "managers/input_manager.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/things/thinkers/3d/light_3d.hpp"
#include "theatre/things/thinkers/3d/camera_3d.hpp"
#include "theatre/things/thinkers/viewport.hpp"
#include "theatre/things/resources/material.hpp"
#include "theatre/things/thinkers/3d/mesh_instance_3d.hpp"
#include "theatre/things/thinkers/3d/collider_3d.hpp"
#include "theatre/things/resources/resource.hpp"
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

static bool sPrintLoadedTheatreData{false};
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
        g_pTheatreManager->LoadNewTheatre(sTheatreFilePath);
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
                        m_AutomaticStopwatchWindow(width);
                        SameLine();
                        m_ManualStopwatchWindow(width);
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
                        m_AutomaticStopwatchWindow(width);
                        SameLine();
                        m_ManualStopwatchWindow(width);
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
    LockGuard<RMutex> lock{mStopwatchMutex};
    if(!sAutoStopwatchEnabled)
        { return StopwatchLog::Invalid; }
    return m_StartStopwatch(message);
}

bool ImGui_Debugger::StopStopwatch(StopwatchLog& stopwatch)
{
    LockGuard<RMutex> lock{mStopwatchMutex};
    if(!sAutoStopwatchEnabled)
        { return false; }
    return m_StopStopwatch(stopwatch);
}

StopwatchLog& ImGui_Debugger::m_StartStopwatch(const std::string& message)
{
    LockGuard<RMutex> lock{mStopwatchMutex};
    StopwatchLog& stopwatch = sStopwatchLogs.emplace_back();
    stopwatch.Start(message);
    return stopwatch;
}

bool ImGui_Debugger::m_StopStopwatch(StopwatchLog& stopwatch)
{
    LockGuard<RMutex> lock{mStopwatchMutex};
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
            Checkbox("Print Theatre Parser Log", &sPrintLoadedTheatreData);
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
        DragFloat("Movement Acceleration", &Settings::Player::MovementAcceleration, 0.01f, 0.0f, 10.0f);
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

void ImGui_Debugger::m_AutomaticStopwatchWindow(float width)
{
    LockGuard<RMutex> lock{mStopwatchMutex};
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

void ImGui_Debugger::m_ManualStopwatchWindow(float width)
{
    LockGuard<RMutex> lock{mStopwatchMutex};
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
// #ifdef DEBUGGING
    // Text("Theatre File Parser Breakpoint:");
    // PushItemWidth(82.0f);
    // InputUInt("Line", &gBreakOnLine, 0, 10, ImGuiInputTextFlags_AutoSelectAll);
    // InputUInt("Column", &gBreakOnColumn, 0, 10, ImGuiInputTextFlags_AutoSelectAll);
    // PushItemWidth(0.0f);
    // Separator();
// #endif // DEBUGGING
    Checkbox("Debug Theatre File Load Printout", &sPrintLoadedTheatreData);

    InputText("Theatre File", &sTheatreFilePath);

    if(IManager::GetTheatreState() != ManagerEnums::NOT_IN_LEVEL)
        { BeginDisabled(); }
    if(Button("Load Theatre"))
    {
        sLastAttemptedTheatreFilePath = sTheatreFilePath;
        g_pTheatreManager->LoadNewTheatre(sTheatreFilePath);
        /*if(sPrintLoadedTheatreData)
            { g_pTheatreManager->CurrentTheatre()->InitialState().debug_PrintData(); }*/
    }
    if(IManager::GetTheatreState() != ManagerEnums::NOT_IN_LEVEL)
        { EndDisabled(); }

    static std::string l_TheatreFilePath{"SavedTheatre.nt"};
    if(IManager::GetTheatreState() != ManagerEnums::IN_LEVEL)
        { BeginDisabled(); }
    if(Button("Save Theatre State"))
        { /*FileSystem::try_WriteFileFromString(l_TheatreFilePath, g_pTheatreManager->CurrentTheatre()->CurrentState().formatted());*/ }
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
        if(auto thinker{DCast<Thinker>(ptr)})
        {
            parent = thinker->Parent()[];
            if(auto actor{DCast<Actor3D>(ptr)})
            {
                position = actor->Position();
                rotation = actor->RotationDegrees();
                scale = actor->Scale();
                visible = actor->Visible();
                if(auto visual3d{DCast<Visual3D>(ptr)})
                {
                    layers_status = visual3d->Layers().status();

                    if(auto mesh_instance{DCast<MeshInstance3D>(ptr)})
                    {
                        mesh = mesh_instance->MeshID()[];
                        material_override = mesh_instance->MaterialOverrideID()[];
                        wireframe = mesh_instance->Wireframe();
                    }
                }
                else if(auto camera3d{DCast<Camera3D>(ptr)})
                {
                    layers_status = camera3d->LayersMask().status();
                    is_camera_current = camera3d->Current();
                    fov = camera3d->mFOV;
                    view_near = camera3d->mViewCutoffNear;
                    view_far = camera3d->mViewCutoffFar;
                    env_type = camera3d->mEnvironment.mType;
                }
                else if(auto collider{DCast<Collider3D>(ptr)})
                {
                    motion = static_cast<int>(collider->Motion());
                    shape  = static_cast<int>(collider->Shape());
                    collider_material = collider->Material();
                    activate_on_change = collider->ActivateOnNextChange();
                }
            }
        }
        else if(auto resource{DCast<Resource>(ptr)})
        {
            if(auto mat{DCast<Material>(ptr)})
            {
                diffuseTexture   = mat->DiffuseTextureID()[];
                specularTexture  = mat->SpecularTextureID()[];
                specularStrength = mat->SpecularStrength();
            }
            else if(auto mesh{DCast<Mesh>(ptr)})
            {
                material = mesh->MaterialID()[];
            }
        }
    }
    Shared<Thing> ptr{nullptr};
    uint id{};
    std::string name{};
    glm::vec3 position{},
        rotation{},
        scale{};
    uint parent{},
        mesh{},
        material{},
        material_override{},
        diffuseTexture{},
        specularTexture{},
        debug_mesh_instance{};
    int motion{static_cast<int>(MotionType::None)},
        shape{static_cast<int>(ShapeType::None)};
    float fov{75.0f},
        view_near{0.001f},
        view_far{1000.0f},
        density{02.0f},
        friction{0.0f},
        specularStrength{0.0f};
    bool visible{true},
        wireframe{false},
        activate_on_change{false},
        is_camera_current{false};
    BitMask::StatusArray layers_status{};
    Environment::BackgroundType env_type{Environment::BackgroundType::BG_CLEAR_COLOR};
    ColliderMaterial collider_material{};
};

static void s_ThingTreeBranch(ID inUID)
{
    auto* theatre{g_pTheatreManager->CurrentTheatre()};
    auto children{theatre->GetChildren(inUID)};
    if(TreeNodeEx(theatre->GetThing(inUID)->c_name(),
        (children.empty())
            ? ImGuiTreeNodeFlags_Leaf
            : ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DefaultOpen))
    {
        for(ID child : children)
            { s_ThingTreeBranch(child); }
        TreePop();
    }
}

void ImGui_Debugger::s_InspectTheatreWindow(bool* is_active)
{
    static std::unordered_map<ID, bool> is_hovered{};
    static uint mNewChildUID{0};
    static thing_data_buffer selected{};
    static int sMaxPerRow{3};
    static float thing_button_color[3]{},
        actor_button_color[3]    {0.063f, 0.392f, 0.6f},
        light_button_color[3]    {0.808f, 0.707f, 0.086f},
        visual3d_button_color[3] {0.447f, 0.125f, 0.361f},
        viewport_button_color[3] {0.500f, 0.300f, 0.800f},
        resource_button_color[3] {0.608f, 0.204f, 0.165f},
        camera_button_color[3]   {0.494f, 0.494f, 0.494f};
    static bool show_type_on_button{true};
    if(IManager::GetTheatreState() != ManagerEnums::IN_LEVEL)
        { selected = thing_data_buffer{}; return; }
    else if(sTheatreInspectorActive)
    {
        if(Begin("Theatre Inspector", is_active))
        {
            auto* theatre{g_pTheatreManager->CurrentTheatre()};
            if(CollapsingHeader("Test Graph View"))
            {
                if(TreeNodeEx(theatre->Name().data(), ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DefaultOpen))
                {
                    auto top_level_ids{theatre->GetChildren({})};
                    for(ID uid : top_level_ids)
                        { s_ThingTreeBranch(uid); }
                    TreePop();
                }
            }
            if(CollapsingHeader("Selector Settings"))
            {
                InputInt("Max Buttons Per Row", &sMaxPerRow, 1, 5);
                ColorEdit3("Thing Color",    thing_button_color,    ImGuiColorEditFlags_Float);
                ColorEdit3("Actor3D Color",  actor_button_color,    ImGuiColorEditFlags_Float);
                ColorEdit3("Light Color",    light_button_color,    ImGuiColorEditFlags_Float);
                ColorEdit3("Visual3D Color", visual3d_button_color, ImGuiColorEditFlags_Float);
                ColorEdit3("Viewport Color", viewport_button_color, ImGuiColorEditFlags_Float);
                ColorEdit3("Resource Color", resource_button_color, ImGuiColorEditFlags_Float);
                ColorEdit3("Camera Color",   camera_button_color, ImGuiColorEditFlags_Float);
                Checkbox("Show Type Abbreviation", &show_type_on_button);
            }
            if(CollapsingHeader("Thing Selection", ImGuiTreeNodeFlags_DefaultOpen))
            {
                BeginChild("Buttons", {0,0}, ImGuiChildFlags_AutoResizeY);
                auto uids{g_pTheatreManager->CurrentTheatre()->ThingIDs()};
                int item_counter{0}, name_counter{0};
                for(ID uid : uids)
                {
                    auto thing{theatre->GetThing(uid)};
                    auto resource{DCast<Resource>(thing)};
                    auto thinker{DCast<Thinker>(thing)};
                    auto visual3d{DCast<Visual3D>(thing)};
                    auto actor{DCast<Actor3D>(thing)};
                    ImVec4 push_color{thing_button_color[0],thing_button_color[1],thing_button_color[2],1.0f};
                    ImGuiCol push_color_where{ImGuiCol_Button};
                    std::string type_symbol{"(T) "};
                    if(thinker)
                    {
                        if(actor)
                        {
                            if(DCast<Light3D>(thing))
                            {
                                push_color = {light_button_color[0],light_button_color[1],light_button_color[2],1.0f};
                                type_symbol = "(L) ";
                            }
                            else if(DCast<Camera3D>(thing))
                            {
                                push_color = {camera_button_color[0],camera_button_color[1],camera_button_color[2],1.0f};
                                type_symbol = "(C) ";
                            }
                            else if(visual3d)
                            {
                                push_color = {visual3d_button_color[0],visual3d_button_color[1],visual3d_button_color[2],1.0f};
                                type_symbol = "(V3D) ";
                            }
                            else
                            {
                                push_color = {actor_button_color[0],actor_button_color[1],actor_button_color[2],1.0f};
                                type_symbol = "(A) ";
                            }
                        }
                        else if(DCast<Viewport>(thing))
                        {
                            push_color = {viewport_button_color[0],viewport_button_color[1],viewport_button_color[2],1.0f};
                            type_symbol = "(VP) ";
                        }
                    }
                    else if(resource)
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
                    thing->mIsHoveredInDebugger = (is_hovered[uid] = IsItemHovered())
                        or is_hovered[theatre->GetParent(thing->uid())];
                    PopID();
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
                and g_pTheatreManager->CurrentTheatre()->DestroyThing(selected.ptr->uid()))
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
                    id_change_status = g_pTheatreManager->CurrentTheatre()->ChangeThingID(selected.ptr->uid(), selected.id);
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
                        { reason = std::format("No Thing found with UID#{}", selected.ptr->uid()[]); }
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
                    { selected.ptr->set_name(selected.name); }
            }
            if(selected.ptr)
                { Text("Type - %s", selected.ptr->type().c_name()); }
            static bool collider_update_transform_instantly{false};
            // THINKERS
            if(auto thinker{DCast<Thinker>(selected.ptr)})
            {
                // PARENT
                BeginChild("Parent", {}, ImGuiChildFlags_AutoResizeY);
                    SeparatorText("Parent");
                    if(auto parent_ptr{g_pTheatreManager->CurrentTheatre()->GetThinker(selected.parent)};
                        !parent_ptr->uid().invalid())
                    {
                        TextF("Parent: {} [Type:{}]",
                            parent_ptr->name(),
                            parent_ptr->type().name());
                        SameLine();
                        if(Button("Inspect"))
                            { selected = thing_data_buffer{parent_ptr}; }
                    }
                    else
                        { TextF("Invalid Parent"); }
                    InputUInt("Parent UID", &selected.parent, 0, 0);
                    if(IsItemDeactivatedAfterEdit())
                    {
                        thinker->SetParent(selected.parent);
                        selected = {thinker};
                    }
                EndChild();
                if(auto viewport{DCast<Viewport>(selected.ptr)})
                {
                    TextF("Size: {}", viewport->Size().data_log());
                    TextF("Framebuffer ID: {}", (viewport->Framebuffer()) ? std::to_string(viewport->Framebuffer()->ID()) : "nullptr");
                }
                // 3D ACTORS
                else if(auto actor{DCast<Actor3D>(selected.ptr)})
                {
                    selected.position = actor->Position();
                    selected.rotation = actor->RotationDegrees();
                    selected.scale    = actor->Scale();
                    if(DragGLMv3("Position", &selected.position, 0.05f, -200.0f, 200.0f, "%.2f"))
                        { actor->SetPosition(selected.position); }
                    if(DragGLMv3("Rotation", &selected.rotation, 0.1f, -359.995f, 359.995f, "%.2f", ImGuiSliderFlags_WrapAround))
                        { actor->SetRotationDegrees(selected.rotation); }
                    if(DragGLMv3("Scale", &selected.scale, 0.01f, -100.0f, 100.0f, "%.2f"))
                        { actor->SetScale(selected.scale); }
                    if(Checkbox("Visible", &selected.visible))
                        { actor->SetVisible(selected.visible); }
                    if(auto visual3d{DCast<Visual3D>(selected.ptr)})
                    {
                        Checkbox("Override Editor Highlight", &visual3d->mOverrideEnableDebugHighlight);
                        ColorEditGLMv4("Editor Highlight Color",
                            &visual3d->mDebugHighlight,
                            ImGuiColorEditFlags_Float
                                | ImGuiColorEditFlags_DisplayRGB
                                | ImGuiColorEditFlags_InputRGB);

                        for(int layer_i{BitMask::min}; layer_i < BitMask::max; ++layer_i)
                        {
                            std::string layer_name{std::format("Layer #{:#02}", layer_i+1)};
                            if(Checkbox(layer_name.data(), &selected.layers_status[layer_i]))
                            {
                                visual3d->SetLayers(selected.layers_status);
                                selected = {visual3d};
                            }
                            if((layer_i+1) % 10 and layer_i != BitMask::max - 1)
                                { SameLine(); }
                        }

                        if(auto light{DCast<Light3D>(actor)})
                        {
                            Checkbox("Enabled", &light->mEnabled);
                            ColorEditGLMv3("Color", &light->mColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB);
                            InputFloat("Energy", &light->mEnergy, 0.01f, 0.05f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                            InputFloat("Specular Strength", &light->mSpecularStrength, 0.1f, 0.5f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                            InputFloat("Ambient Strength", &light->mAmbientStrength, 0.01f, 0.05f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                            InputFloat("Attenuation Scalar", &light->mAttenuation, 0.01f, 0.05f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                            InputFloat("Range", &light->mRange, 1.0f, 0.5f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                            if(DCast<SpotLight3D>(light))
                            {
                                InputFloat("SpotAngle", &light->mSpotAngle, 0.1f, 0.5f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                                InputFloat("SpotAngleFade", &light->mSpotAngleFade, 0.5f, 1.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                            }
                        }
                        else if(auto mesh_instance{DCast<MeshInstance3D>(selected.ptr)})
                        {
                            if(InputUInt("Mesh UID", &selected.mesh, 0, 0))
                                { mesh_instance->SetMeshID(selected.mesh); }
                            if(InputUInt("Material Override UID", &selected.material_override, 0, 0))
                                { mesh_instance->SetMaterialOverrideID(selected.material); }
                            if(Checkbox("Wireframe", &selected.wireframe))
                                { mesh_instance->SetWireframe(selected.wireframe); }
                            if(IsItemHovered())
                                { SetTooltip("%s", "Enabling the global wireframe setting will override this option"); }
                        }
                    }
                    else if(auto camera3d{DCast<Camera3D>(selected.ptr)})
                    {
                        for(int layers_mask_i{BitMask::min}; layers_mask_i < BitMask::max; ++layers_mask_i)
                        {
                            std::string layer_name{std::format("Layer #{:#02}", layers_mask_i+1)};
                            if(Checkbox(layer_name.data(), &selected.layers_status[layers_mask_i]))
                            {
                                camera3d->SetLayersMask(selected.layers_status);
                                selected = {camera3d};
                            }
                            if((layers_mask_i+1) % 10 and layers_mask_i != BitMask::max - 1)
                                { SameLine(); }
                        }
                        auto cur_vp{g_pTheatreManager->CurrentTheatre()->GetThinker<Viewport>(camera3d->ViewportID())};
                        TextF("Viewport: {} [{}]", cur_vp->name(), cur_vp->uid()[]);
                        if(Checkbox("Current", &selected.is_camera_current))
                        {
                            print_error_enum(camera3d->SetCurrent(selected.is_camera_current));
                            selected = {camera3d};
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
                        if(BeginCombo("Environment", bg_type.at(current_type)))
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
                    else if(auto collider{DCast<Collider3D>(selected.ptr)})
                    {
                        if(CollapsingHeader("Jolt Properties"))
                        {
                            auto bodyid{collider->id()};
                            auto& interface{PhysicsEngine::Inst()->BodyInterface()};
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
                                scale.GetX(),
                                scale.GetY(),
                                scale.GetZ());
                        }
                        static const char* motion_names{"Static\0Dynamic\0Kinematic\0None\0"};
                        static const char* shape_names{"Box\0Sphere\0Capsule\0Cylinder\0None\0"};
                        if(InputFloat("Mass", &selected.density, 0.1f, 1.0f))
                            { collider->SetMass(selected.density); }
                        if(InputFloat("Friction", &selected.collider_material.friction, 0.01f, 0.1f))
                        {
                            if(selected.collider_material.friction > 1.0f)
                                { selected.collider_material.friction = 1.0f; }
                            else if(selected.collider_material.friction < 0.0f)
                                { selected.collider_material.friction = 0.0f; }
                            collider->SetMaterial(selected.collider_material);
                        }
                        if(Combo("Motion", &selected.motion, motion_names))
                            { collider->SetMotion((MotionType)selected.motion, selected.activate_on_change); }
                        if(Combo("Shape", &selected.shape, shape_names))
                            { collider->SetShape((ShapeType)selected.shape, selected.activate_on_change); }
                        if(Checkbox("Set Active on Change", &selected.activate_on_change))
                            { collider->SetActivateOnNextChange(selected.activate_on_change); selected = {collider}; }
                        Checkbox("Update Transform Values Instantly", &collider_update_transform_instantly);
                        TextF("Active: {}", collider->Active());
                        if(!collider->Active() and Button("Activate"))
                            { collider->SetActive(true); }
                        else if(collider->Active() and Button("Deactivate"))
                            { collider->SetActive(false); }
                    }
                }
                // CHILDREN
                BeginChild("Children", {}, ImGuiChildFlags_AutoResizeY);
                    SeparatorText("Children");
                    auto children{thinker->Children()};
                    if(Button("+"))
                    {
                        print_error_enum(g_pTheatreManager->CurrentTheatre()
                            ->SetParent(mNewChildUID, thinker->uid()));
                        selected = {thinker};
                        mNewChildUID = 0;
                    }
                    SameLine();
                    InputUInt("UID##2", &mNewChildUID, 0, 0);
                    uint child_counter{0};
                    for(FAUTO child : children)
                    {
                        if(child.invalid())
                            { TextF("[Invalid Child UID]##{}", ++child_counter); }
                        else
                        {
                            auto thing{g_pTheatreManager->CurrentTheatre()->GetThing(child)};
                            PushID(++child_counter + thing->uid()[]);
                            if(Button("-"))
                            {
                                print_error_enum(g_pTheatreManager->CurrentTheatre()
                                    ->DropParent(child));
                                selected = {selected.ptr};
                                PopID(); EndChild(); EndChild(); End(); return;
                            }
                            SameLine();
                            TextF("Child: {} [Type:{}] [UID:{}]",
                                thing->name(),
                                thing->type().name(),
                                thing->uid()[]);
                            SameLine();
                            if(Button("Inspect"))
                                { selected = thing_data_buffer{thing}; }
                            PopID();
                        }
                    }
                EndChild();
            }
            // RESOURCES
            else if(auto resource{DCast<Resource>(selected.ptr)})
            {
                if(auto mesh{DCast<Mesh>(selected.ptr)})
                {
                    if(InputUInt("Material UID", &selected.material, 0, 0))
                        { mesh->MaterialID(selected.material); }
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
                    if(InputFloat("Specular Strength", &selected.specularStrength, 0.05f, 0.1f, "%.3f"))
                        { material->SpecularStrength(selected.specularStrength); selected = {material}; }
                }
            }
            EndChild();
        }
        End();
    }
}
