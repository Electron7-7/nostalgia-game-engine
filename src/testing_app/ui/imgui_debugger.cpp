#include "imgui_debugger.hpp"
#include "../app/nostalgia_goggles.hpp"
#include "managers/manager.hpp"
#include "managers/input_manager.hpp"
#include "demo/demo_controller.hpp"
#include "managers/backend_manager.hpp"
#include "settings/settings.hpp"
#include "managers/theatre_manager.hpp"
#include "DearImGui/imgui.h"
#include "DearImGui/imgui_stdlib.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

#ifdef DEBUGGING
#   include "time.hpp"
#   include "things/thing_factory.hpp"
#   include "rendering/backends/graphics/opengl.hpp"
#   include "theatre_parser/theatre_parser.hpp"
#   include "things/actors/light.hpp"
#   include "things/devices/mesh_instance.hpp"
#   include "things/devices/material.hpp"
#   include "things/resources/resource.hpp"
#   include "embedded/names.hpp"

#   include <set>
#   include <format>
#   include <memory>
#   include <random>

static std::random_device s_RandomDevice;

static bool s_TheatreInspectorActive = false;
static bool s_AutoStopwatchEnabled = true;

static std::vector<TheatreLog>   s_TheatreLogs = {};
static std::vector<StopwatchLog> s_ManualStopwatchLogs = {};
static std::vector<StopwatchLog> s_StopwatchLogs = {};
static std::set<int>             s_StopwatchLogIds = {};

static void s_AutomaticStopwatchWindow(float);
static void s_ManualStopwatchWindow(float);
static void s_TheatreDebuggingWindow();
#endif // DEBUGGING
static void s_GeneralDebuggingWindow();

using namespace ImGui;

static std::string s_TheatreFilePath = "theatres/HelloWorld.nt";
static std::string s_LastAttemptedTheatreFilePath = s_TheatreFilePath;
static std::string sDemoFileOut = "demo";
static std::string sDemoFileIn = std::format("{}.dem", sDemoFileOut);

static imgui_Debugger s_Debugger;
imgui_Debugger* g_pDebugger = &s_Debugger;

static void s_GeneralDebuggingWindow()
{
#ifndef DEBUGGING
    BeginChild("Settings");
#else  // DEBUGGING
    BeginChild("General Debugging");
    if(CollapsingHeader("Messages"))
    {
        SeparatorText("General");
            Checkbox("Print Frame#", &g_PrintFrameNumbers);
            SameLine();
            Checkbox("Print Tick#", &g_PrintTickNumbers);
        if(Settings::Engine::GraphicsBackend == gBackendIDs::gOpenGL)
        {
            SeparatorText("OpenGL");
            Text("Severity Levels");
            Separator();
            Checkbox("High", &g_EnableDebugMsgHigh); SameLine();
            Checkbox("Medium", &g_EnableDebugMsgMedium); SameLine();
            Checkbox("Low", &g_EnableDebugMsgLow); SameLine();
            Checkbox("Notification", &g_EnableDebugMsgNotif);
        }
    }

    if(CollapsingHeader("Rendering"))
    {
        Checkbox("Global Wireframe Mode", &Settings::Graphics::GlobalWireframe);
        Text("Shader Output");
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
        g_ShaderDebugOuptut = s_Selected;
    }
    if(CollapsingHeader("Engine"))
    {
        DragInt("Tick Rate", &Settings::Engine::TickRate);
        Text("Tick Interval: %f", Settings::Engine::TickInterval());
        Text("Graphics Backend: %d", Settings::Engine::GraphicsBackend);
        Text("Windowing Backend: %d", Settings::Engine::WindowingBackend);
    }
#endif // DEBUGGING
    if(CollapsingHeader("Window"))
    {
        if(Checkbox("Fullscreen", &Settings::Window::Fullscreen))
            { g_pBackendManager->Windowing()->SetFullscreen(Settings::Window::Fullscreen); }
        int l_Pos[2]  = { Settings::Window::XPosition, Settings::Window::YPosition };
        int l_Size[2] = { Settings::Window::Width, Settings::Window::Height };
        if(Settings::Window::Fullscreen)
        {
            l_Pos[0] = Settings::Window::FullscreenXPosition;
            l_Pos[1] = Settings::Window::FullscreenYPosition;
            l_Size[0] = Settings::Window::FullscreenWidth;
            l_Size[1] = Settings::Window::FullscreenHeight;
        }
        InputInt2("Position", l_Pos);
        if(IsItemDeactivatedAfterEdit())
            { g_pBackendManager->Windowing()->MoveWindow(l_Pos[0], l_Pos[1]); }
        InputInt2("Size", l_Size);
        if(IsItemDeactivatedAfterEdit())
            { g_pBackendManager->Windowing()->ResizeWindow(l_Size[0], l_Size[1]); }
        NewLine();
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
        if(Checkbox("Raw Mouse Motion", &Settings::Player::RawMouseMotion))
            { g_pBackendManager->Windowing()->SetRawMouseMotion(Settings::Player::RawMouseMotion); }
        SliderFloat("Mouse Sensitivity", &Settings::Player::MouseSensitivity, 0.0f, 5.0f);
        SliderFloat("Mouse Sensitivity Multiplier", &Settings::Player::MouseSensitivityScale, 0.0f, 1.0f);
    }
    EndChild();
}

bool imgui_Debugger::Init()
{
    if(m_IsInitialized)
        { return true; }
    m_IsInitialized = true;
    return true;
}

void imgui_Debugger::Shutdown()
{
    if(!m_IsInitialized)
        { return; }
    m_IsInitialized = false;
}

void imgui_Debugger::Update()
{
    static bool show_demo_window = false;
    if(show_demo_window)
        { ShowDemoWindow(&show_demo_window); }
    if(IsKeyDown(ImGuiMod_Ctrl) && IsKeyPressed(ImGuiKey_G))
        { show_demo_window = !show_demo_window; }
    if(IsKeyDown(ImGuiMod_Ctrl) && IsKeyPressed(ImGuiKey_Q))
        { g_pApplication->Shutdown(); }
    if(IsKeyDown(ImGuiMod_Super) && IsKeyPressed(ImGuiKey_Q))
        { g_pApplication->Shutdown(); }

    static bool show_main_window = true;
    if(IsKeyPressed(ImGuiKey_Escape))
        { show_main_window = !show_main_window; }

#ifndef DEBUGGING
    SetNextWindowSize({854,443}, ImGuiCond_FirstUseEver);
#else // DEBUGGING
    static bool s_PopOutStopwatches = false;
    if(s_TheatreInspectorActive)
        { s_InspectTheatreWindow(&s_TheatreInspectorActive); }
    SetNextWindowSize({840,530}, ImGuiCond_FirstUseEver);
#endif
    if(show_main_window)
    {
        if(Begin("Debugging", &show_main_window, ImGuiWindowFlags_MenuBar))
        {
            if(BeginMenuBar())
            {
                if(BeginMenu("Menu"))
                {
                    if(MenuItem("Show ImGui Demo Window", "CTRL+G"))
                        { show_demo_window = true; }
                    if(MenuItem("Quit", "CTRL+Q"))
                        { g_pApplication->Shutdown(); }
                    EndMenu();
                }
                EndMenuBar();
#ifdef DEBUGGING
            }
            if(BeginTabBar("Debug Tools"))
            {
                if(BeginTabItem("General"))
                {
                    s_GeneralDebuggingWindow();
                    EndTabItem();
                }
                if(s_PopOutStopwatches)
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
                    const char* pop_out_label = (s_PopOutStopwatches) ? "Bring Back Window" : "Pop Out Window";
                    if(Button(pop_out_label))
                        { s_PopOutStopwatches = !s_PopOutStopwatches; }
                    if(!s_PopOutStopwatches)
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
#else  // !DEBUGGING
                Text("%s", "You are currently testing the 'Release' version of Nostalgia.\nIf you want all the debug features, please compile the 'Debug' version of both Nostalgia and the included testing app.\nThis can be done with 'make debug static testapp_static'.");

                InputText("Theatre File", &s_TheatreFilePath);
                if(_Manager::GetTheatreState() != ManagerEnums::NOT_IN_LEVEL)
                    { BeginDisabled(); }
                if(Button("Load Theatre"))
                {
                    s_LastAttemptedTheatreFilePath = s_TheatreFilePath;
                    TheatreManager::LoadTheatreFromFile(s_TheatreFilePath);
                }
                if(_Manager::GetTheatreState() != ManagerEnums::NOT_IN_LEVEL)
                    { EndDisabled(); }

                if(_Manager::GetTheatreState() != ManagerEnums::IN_LEVEL)
                    { BeginDisabled(); }
                if(Button("Exit Theatre"))
                    { _Manager::ShutdownTheatre(); }
                if(_Manager::GetTheatreState() != ManagerEnums::IN_LEVEL)
                    { EndDisabled(); }
                NewLine();
                SeparatorText("Settings");
                s_GeneralDebuggingWindow();
            }
#endif // DEBUGGING
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

#ifdef DEBUGGING
void s_HandleAutomaticStopwatchToggle()
{
    s_AutoStopwatchEnabled = !s_AutoStopwatchEnabled;

    if(s_AutoStopwatchEnabled)
        { return; }

    for(StopwatchLog& log : s_StopwatchLogs)
    {
        if(log.Stop())
            { log.m_Message = "Interrupted by disabling automatic stopwatches"; }
    }
}

StopwatchLog StopwatchLog::Invalid(true);

bool StopwatchLog::Start(const std::string& message)
{
    if(m_IsRunning || m_IsFinished)
        { return false; }
    m_IsRunning    = true;
    m_IsFinished   = false;
    m_Message = message;

    m_FormattedStartTime = Time::CurrentFormatted();
    m_HighResStartTime   = Time::Current();
    m_HighResStopTime    = m_HighResStartTime;
    return true;
}

bool StopwatchLog::Stop()
{
    if(!m_IsRunning || m_IsFinished)
        { return false; }
    m_IsRunning   = false;
    m_IsFinished  = true;
    m_HighResStopTime = Time::Current();
    m_FormattedStopTime = Time::CurrentFormatted();
    return true;
}

const std::string& StopwatchLog::GetStartTime() const
{ return m_FormattedStartTime; }

const std::string& StopwatchLog::GetStopTime() const
{ return m_FormattedStopTime; }

double StopwatchLog::GetHiResStartTime() const
{ return m_HighResStartTime; }

double StopwatchLog::GetHiResStopTime() const
{ return m_HighResStopTime; }

double StopwatchLog::GetDuration() const
{
    if(m_IsRunning || !m_IsFinished)
        { return (Time::Current() - m_HighResStartTime); }
    return (m_HighResStopTime - m_HighResStartTime);
}

const std::string& StopwatchLog::GetMessage() const
{ return m_Message; }

bool StopwatchLog::IsRunning() const
{ return m_IsRunning; }

bool StopwatchLog::IsFinished() const
{ return m_IsFinished; }

StopwatchLog& imgui_Debugger::StartStopwatch(const std::string& message)
{
    if(!s_AutoStopwatchEnabled)
        { return StopwatchLog::Invalid; }
    return m_StartStopwatch(message);
}

bool imgui_Debugger::StopStopwatch(StopwatchLog& stopwatch)
{
    if(!s_AutoStopwatchEnabled)
        { return false; }
    return m_StopStopwatch(stopwatch);
}

StopwatchLog& imgui_Debugger::m_StartStopwatch(const std::string& message)
{
    #pragma message("TODO: Make this thread safe")
    StopwatchLog& stopwatch = s_StopwatchLogs.emplace_back();
    stopwatch.Start(message);
    return stopwatch;
}

bool imgui_Debugger::m_StopStopwatch(StopwatchLog& stopwatch)
{
    if(stopwatch == StopwatchLog::Invalid || !stopwatch.IsRunning())
        { return false; }
    if(std::find(s_StopwatchLogs.begin(), s_StopwatchLogs.end(), stopwatch) == s_StopwatchLogs.end())
        { return false; }

    stopwatch.Stop();
    return true;
}

void imgui_Debugger::StartTheatreTiming(bool loading)
{
    if(loading)
    {
        TheatreLog& log = s_TheatreLogs.emplace_back();
        log.m_LoadTime.Start();
        log.m_TheatreFilePath = s_LastAttemptedTheatreFilePath;
    }
    else
    {
        if(s_TheatreLogs.empty())
            { return; }
        TheatreLog& log = s_TheatreLogs.back();
        log.m_UnloadTime.Start();
        if(log.m_LoadTime.IsRunning())
            { log.m_LoadTime.Stop(); }
    }
}

void imgui_Debugger::StopTheatreTiming(bool loading)
{
    if(s_TheatreLogs.empty())
        { return; }
    TheatreLog& log = s_TheatreLogs.back();
    if(loading)
        { log.m_LoadTime.Stop(); }
    else
        { log.m_UnloadTime.Stop(); }
}

static void s_PrintStopwatchLog(const StopwatchLog& stopwatch, size_t i)
{
    if(!stopwatch.IsFinished())
        { return; }
    Separator();
    Text("Stopwatch #%zu", i);
    Text("Time: %f", stopwatch.GetDuration());
    if(IsItemHovered())
        { SetTooltip("Started @ %s (hi-res: %f)\nStopped @ %s (hi-res: %f)", stopwatch.GetStartTime().c_str(), stopwatch.GetHiResStartTime(), stopwatch.GetStopTime().c_str(), stopwatch.GetHiResStopTime()); }
    Text("Message: '%s'", stopwatch.GetMessage().c_str());
}

static void s_AutomaticStopwatchWindow(float width)
{
    BeginChild("Automatic Stopwatch", {width, 0}, ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_Borders);
    Text("%s", "Automatic Stopwatch");
    static bool s_DummyAutoStopwatchEnabled = true;
    Checkbox("Enable Automatic Stopwatch", &s_DummyAutoStopwatchEnabled);
    if(IsItemHovered())
        { SetTooltip("%s", "This enables/disables the program's ability to create new stopwatches"); }
    if(s_DummyAutoStopwatchEnabled != s_AutoStopwatchEnabled)
        { s_HandleAutomaticStopwatchToggle(); }

    if(Button("Clear Logs"))
        { s_StopwatchLogs.clear(); }

    BeginGroup();
        static bool sort_newest_first = false;
        ImGuiDir button_arrow_direction = (sort_newest_first) ? ImGuiDir_Down : ImGuiDir_Up;

        Text("Sorting by: %s", (sort_newest_first) ? "Newest" : "Oldest");
        SameLine();
        if(ArrowButton("SortingDirection", button_arrow_direction))
            { sort_newest_first = !sort_newest_first; }
    EndGroup();

    Text("Logs:");
    if(!s_StopwatchLogs.empty())
    {
        std::vector<StopwatchLog> logs = {s_StopwatchLogs.cbegin(), s_StopwatchLogs.cend()};

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
        if(s_ManualStopwatchLogs.empty() || !s_ManualStopwatchLogs.back().IsRunning())
            { s_ManualStopwatchLogs.emplace_back().Start(); }
    }
    if(Button("Stop Stopwatch"))
    {
        if(!s_ManualStopwatchLogs.empty() && s_ManualStopwatchLogs.back().IsRunning())
            { s_ManualStopwatchLogs.back().Stop(); }
    }

    std::string last_stopwatch_time = (s_ManualStopwatchLogs.empty()) ? "N/A" : std::to_string(s_ManualStopwatchLogs.back().GetDuration());
    Text("Most Recent Stopwatch: %s", last_stopwatch_time.c_str());

    if(Button("Clear Logs"))
        { s_ManualStopwatchLogs.clear(); }
    Text("Logs:");

    if(!s_ManualStopwatchLogs.empty())
    {
        Separator();
        for(size_t i = 0 ; i < s_ManualStopwatchLogs.size() ; ++i)
        {
            const StopwatchLog& stopwatch = s_ManualStopwatchLogs.at(i);
            Text("Manual Stopwatch #%zu", i);
            Text("Time: %f", stopwatch.GetDuration());
            if(IsItemHovered())
            {
                std::string tooltip_extra = (stopwatch.IsFinished()) ? std::format("{}", stopwatch.GetStopTime()) : "N/A";
                SetTooltip("Started @ %s\nStopped @ %s", stopwatch.GetStartTime().c_str(), tooltip_extra.c_str());
            }
        }
    }
    EndChild();
}

static void s_TheatreDebuggingWindow()
{
    BeginChild("Theatre Debugging");
    Text("Theatre File Parser Breakpoint:");
    PushItemWidth(82.0f);
    InputInt("Line", &g_BreakOnLine, 0, 10, ImGuiInputTextFlags_AutoSelectAll);
    InputInt("Column", &g_BreakOnColumn, 0, 10, ImGuiInputTextFlags_AutoSelectAll);
    PushItemWidth(0.0f);
    Separator();

    InputText("Theatre File", &s_TheatreFilePath);

    if(_Manager::GetTheatreState() != ManagerEnums::NOT_IN_LEVEL)
        { BeginDisabled(); }
    if(Button("Load Theatre"))
    {
        s_LastAttemptedTheatreFilePath = s_TheatreFilePath;
        TheatreManager::LoadTheatreFromFile(s_TheatreFilePath);
    }
    if(_Manager::GetTheatreState() != ManagerEnums::NOT_IN_LEVEL)
        { EndDisabled(); }

    if(_Manager::GetTheatreState() != ManagerEnums::IN_LEVEL)
        { BeginDisabled(); }
    if(Button("Exit Theatre"))
        { _Manager::ShutdownTheatre(); }
    if(_Manager::GetTheatreState() != ManagerEnums::IN_LEVEL)
        { EndDisabled(); }

    bool not_in_level = (_Manager::GetTheatreState() == ManagerEnums::NOT_IN_LEVEL);

    if(not_in_level)
    {
        BeginDisabled();
        s_TheatreInspectorActive = false;
    }
    if(Button("Inspect Theatre"))
        { s_TheatreInspectorActive = !s_TheatreInspectorActive; }
    if(not_in_level)
        { EndDisabled(); }

    if(Button("Clear Logs"))
        { s_TheatreLogs.clear(); }
    Text("Logs:");

    for(const TheatreLog& log : s_TheatreLogs)
    {
        Separator();
        Text("Theatre File: %s", log.m_TheatreFilePath.c_str());
        if(log.m_LoadTime.IsFinished())
        {
            Text("Load Time: %fs", log.m_LoadTime.GetDuration());
            if(IsItemHovered())
                { SetTooltip("Started @ %s\nStopped @ %s", log.m_LoadTime.GetStartTime().c_str(), log.m_LoadTime.GetStopTime().c_str()); }
        }
        if(log.m_UnloadTime.IsFinished())
        {
            Text("Unload Time: %fs", log.m_UnloadTime.GetDuration());
            if(IsItemHovered())
                { SetTooltip("Started @ %s\nStopped @ %s", log.m_UnloadTime.GetStartTime().c_str(), log.m_UnloadTime.GetStopTime().c_str()); }
        }
    }
    EndChild();
}

static bool s_GetImageName(std::string& out, id_t id)
{
    switch(id)
    {
    case UniqueIDs::Reserved::i_Missing:
        out = Images::Name::Missing;
        break;
    case UniqueIDs::Reserved::i_COMP04_5:
        out = Images::Name::COMP04_5;
        break;
    case UniqueIDs::Reserved::i_LolBit:
        out = Images::Name::LolBit;
        break;
    case UniqueIDs::Reserved::i_LightDebug:
        out = Images::Name::LightDebug;
        break;
    default:
        return false;
    }
    return true;
}

static bool s_GetModelName(std::string& out, id_t id)
{
    switch(id)
    {
    case UniqueIDs::Reserved::m_Error:
        out = Models::Name::Error;
        break;
    case UniqueIDs::Reserved::m_Ramiel:
        out = Models::Name::Ramiel;
        break;
    case UniqueIDs::Reserved::m_Cube:
        out = Models::Name::Cube;
        break;
    default:
        return false;
    }
    return true;
}

void imgui_Debugger::s_InspectTheatreWindow(bool* is_active)
{
    static std::shared_ptr<Thing> s_Thing = nullptr;
    if(Begin("Theatre Inspector", is_active))
    {
        static float width_scaler = 1.0f;
        static int s_MaxPerRow = 3;
        static bool s_Highlight = false;
        if(CollapsingHeader("Thing Selection", ImGuiTreeNodeFlags_DefaultOpen))
        {
            PushItemFlag(ImGuiItemFlags_NoNavDefaultFocus, true);
            InputInt("Max Buttons Per Row", &s_MaxPerRow, 1, 5);
            PopItemFlag();
            BeginChild("Buttons", {0,0}, ImGuiChildFlags_AutoResizeY);
            std::shared_ptr<Thing> thing;
            int i = -1;
            while(TheatreManager::debug_GetThingAtIndex(++i, thing))
            {
                if(std::dynamic_pointer_cast<Resource>(thing) || std::dynamic_pointer_cast<MeshInstance>(thing))
                    { continue; }
                s_Highlight = false;
                bool push_color = false;
                if(std::dynamic_pointer_cast<light_t>(thing))
                    { PushStyleColor(ImGuiCol_Button, IM_COL32(100, 103, 48, 255)); push_color = true; }
                else if(std::dynamic_pointer_cast<Actor>(thing))
                    { PushStyleColor(ImGuiCol_Button, IM_COL32(103, 48, 101, 255)); push_color = true; }
                if(Button(thing->name().c_str(), {(GetWindowWidth() / s_MaxPerRow) - 5.0f, 0.0f}))
                {
                    s_Thing = thing;
                    s_Highlight = true;
                }
                if(push_color)
                    { PopStyleColor(); }
                if(IsItemHovered())
                    { s_Highlight = true; }
                if(std::dynamic_pointer_cast<Actor>(thing))
                {
                    TheatreManager::GetThing<Material>(
                        TheatreManager::GetThing<MeshInstance>(
                            std::dynamic_pointer_cast<Actor>(
                                thing)->GetMeshInstanceID()
                            )->GetMaterialID()
                        )->mDebugHighlight.a = s_Highlight;
                }
                if(++i < s_MaxPerRow)
                    { SameLine(); }
                else
                    { i = 0; }
            }
            EndChild();
        }
        auto actor = std::dynamic_pointer_cast<Actor>(s_Thing);
        auto light = std::dynamic_pointer_cast<light_t>(s_Thing);
        auto material = std::dynamic_pointer_cast<Material>(s_Thing);

        if(s_Thing && _Manager::GetTheatreState() == ManagerEnums::IN_LEVEL)
        {
            BeginChild("View Thing", {0,0}, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border);
            if(TreeNodeEx("Immutable Properties", ImGuiTreeNodeFlags_DefaultOpen))
            {
                Text("Name: '%s'", s_Thing->name().c_str());
                Text("Type: (%s)", ThingFactory::GetTypeName(s_Thing->type()).c_str());
                Text("ID: [%u]", (id_t)s_Thing->uid());
                if(actor)
                {
                    Text("Quaternion: (%f, %f, %f, %f)", actor->Quaternion().w, actor->Quaternion().x, actor->Quaternion().y, actor->Quaternion().z);
                    if(IsItemHovered())
                        { SetTooltip("%s", "(w, x, y, z)"); }
                }
                TreePop();
            }
            if(TreeNode("Member Info"))
            {
                if(actor)
                {
                    auto mInst = TheatreManager::GetThing<MeshInstance>(actor->GetMeshInstanceID());
                    auto mat = TheatreManager::GetThing<Material>(mInst->GetMaterialID());
                    PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0,0,0,0));
                    PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0,0,0,0));
                    static ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_Leaf;
                    if(TreeNodeEx("MeshInstance", tree_flags, "MeshInstance - '%s' [%u]", mInst->name().c_str(), (id_t)actor->GetMeshInstanceID()))
                    {
                        std::string mesh_name = TheatreManager::GetThing(mInst->GetMeshID())->name();
                        s_GetModelName(mesh_name, mInst->GetMeshID());
                        TreeNodeEx("Mesh", tree_flags | ImGuiTreeNodeFlags_NoTreePushOnOpen, "Mesh - '%s' [%u]", mesh_name.c_str(), (id_t)mInst->GetMeshID());
                        if(TreeNodeEx("Material", tree_flags, "Material - '%s' [%u]", TheatreManager::GetThing(mInst->GetMaterialID())->name().c_str(), (id_t)mInst->GetMaterialID()))
                        {
                            std::string diffuse_name  = TheatreManager::GetThing(mat->GetDiffuseTexture())->name();
                            std::string specular_name = TheatreManager::GetThing(mat->GetSpecularTexture())->name();
                            s_GetImageName(diffuse_name, mat->GetDiffuseTexture());
                            s_GetImageName(specular_name, mat->GetSpecularTexture());
                            TreeNodeEx("DiffuseTexture",  tree_flags | ImGuiTreeNodeFlags_NoTreePushOnOpen, "DiffuseTexture - '%s' [%u]", diffuse_name.c_str(), (id_t)mat->GetDiffuseTexture());
                            TreeNodeEx("SpecularTexture", tree_flags | ImGuiTreeNodeFlags_NoTreePushOnOpen, "SpecularTexture - '%s' [%u]", specular_name.c_str(), (id_t)mat->GetSpecularTexture());
                            TreePop();
                        }
                        TreePop();
                    }
                    PopStyleColor();
                    PopStyleColor();
                }
                if(material)
                {
                    std::string diffuse_name  = TheatreManager::GetThing(material->GetDiffuseTexture())->name();
                    std::string specular_name = TheatreManager::GetThing(material->GetSpecularTexture())->name();
                    s_GetImageName(diffuse_name, material->GetDiffuseTexture());
                    s_GetImageName(specular_name, material->GetSpecularTexture());
                    Text("DiffuseTexture - '%s' [%u]", diffuse_name.c_str(), (id_t)material->GetDiffuseTexture());
                    Text("SpecularTexture - '%s' [%u]", specular_name.c_str(), (id_t)material->GetSpecularTexture());
                }
                TreePop();
            }
            EndChild();

            int max_size = 0;
            if(actor || light || material)
            {
                BeginChild("Edit Thing", {0,0}, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border);
                Text("Mutable Properties");
                Separator();

                std::string string = "";
                if(material)
                {
                    Checkbox("Don't Use Textures", &material->mDontUseTexture);
                    max_size = 19;
                    width_scaler = 8.2f;
                }

                if(light)
                {
                    Checkbox("Enabled", &light->mEnabled);
                    SameLine();
                }

                if(actor)
                {
                    max_size = 9;
                    width_scaler = 9.6f;
                    Checkbox("Visible", &actor->mVisible);
                    if(!light)
                    {
                        SameLine();
                        Checkbox("Actor Wireframe", &actor->mWireframe);
                        if(IsItemHovered())
                            { SetTooltip("%s", "Enabling the global wireframe setting will override this option"); }
                    }
                }

                if(light)
                {
                    max_size = 17;
                    width_scaler = 8.4f;
                    string = "Color:";
                    string.insert(string.end(), max_size - string.size(), ' ');
                    Text("%s", string.c_str());
                    SameLine();
                    SetNextItemWidth(GetWindowWidth() - (max_size * width_scaler));
                    ColorEditGLMv3("##color", &light->mColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB);

                    string = "Energy:";
                    string.insert(string.end(), max_size - string.size(), ' ');
                    Text("%s", string.c_str());
                    SameLine();
                    SetNextItemWidth(GetWindowWidth() - (max_size * width_scaler));
                    InputFloat("##Energy", &light->mEnergy, 0.01f, 0.05f, "%.3f", ImGuiInputTextFlags_CharsDecimal);

                    string = "SpecularStrength:";
                    string.insert(string.end(), max_size - string.size(), ' ');
                    Text("%s", string.c_str());
                    SameLine();
                    SetNextItemWidth(GetWindowWidth() - (max_size * width_scaler));
                    InputFloat("##SpecularStrength", &light->mSpecularStrength, 0.1f, 0.5f, "%.3f", ImGuiInputTextFlags_CharsDecimal);

                    string = "AmbientStrength:";
                    string.insert(string.end(), max_size - string.size(), ' ');
                    Text("%s", string.c_str());
                    SameLine();
                    SetNextItemWidth(GetWindowWidth() - (max_size * width_scaler));
                    InputFloat("##AmbientStrength", &light->mAmbientStrength, 0.01f, 0.05f, "%.3f", ImGuiInputTextFlags_CharsDecimal);

                    string = "Attenuation:";
                    string.insert(string.end(), max_size - string.size(), ' ');
                    Text("%s", string.c_str());
                    SameLine();
                    SetNextItemWidth(GetWindowWidth() - (max_size * width_scaler));
                    InputFloat("##Attenuation", &light->mAttenuation, 0.01f, 0.05f, "%.3f", ImGuiInputTextFlags_CharsDecimal);

                    string = "Range:";
                    string.insert(string.end(), max_size - string.size(), ' ');
                    Text("%s", string.c_str());
                    SameLine();
                    SetNextItemWidth(GetWindowWidth() - (max_size * width_scaler));
                    InputFloat("##Range", &light->mRange, 1.0f, 0.5f, "%.3f", ImGuiInputTextFlags_CharsDecimal);

                    if(std::dynamic_pointer_cast<SpotLight>(light))
                    {
                        string = "SpotAngle:";
                        string.insert(string.end(), max_size - string.size(), ' ');
                        Text("%s", string.c_str());
                        SameLine();
                        SetNextItemWidth(GetWindowWidth() - (max_size * width_scaler));
                        InputFloat("##SpotAngle", &light->mSpotAngle, 0.1f, 0.5f, "%.3f", ImGuiInputTextFlags_CharsDecimal);

                        string = "SpotAngleFade:";
                        string.insert(string.end(), max_size - string.size(), ' ');
                        Text("%s", string.c_str());
                        SameLine();
                        SetNextItemWidth(GetWindowWidth() - (max_size * width_scaler));
                        InputFloat("##SpotAngleFade", &light->mSpotAngleFade, 0.5f, 1.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
                    }
                }

                if(actor)
                {
                    string = "Origin:";
                    string.insert(string.end(), max_size - string.size(), ' ');
                    Text("%s", string.c_str());
                    SameLine();
                    SetNextItemWidth(GetWindowWidth() - (max_size * width_scaler));
                    DragGLMv3("##origin", &actor->Origin(), 0.05f, -200.0f, 200.0f, "%.2f");

                    glm::vec3 Euler = actor->Euler(true);

                    string = "Rotation:";
                    string.insert(string.end(), max_size - string.size(), ' ');
                    Text("%s", string.c_str());
                    SameLine();
                    SetNextItemWidth(GetWindowWidth() - (max_size * width_scaler));
                    if(DragGLMv3("##rotation", &Euler, 0.1f, -359.995f, 359.995f, "%.2f", ImGuiSliderFlags_WrapAround))
                        { actor->Euler(Euler, true); }

                    string = "Scale:";
                    string.insert(string.end(), max_size - string.size(), ' ');
                    Text("%s", string.c_str());
                    SameLine();
                    SetNextItemWidth(GetWindowWidth() - (max_size * width_scaler));
                    DragGLMv3("##scale", &actor->Scale(), 0.01f, -100.0f, 100.0f, "%.2f");
                }

                if(material)
                {
                    string = "Color:";
                    string.insert(string.end(), max_size - string.size(), ' ');
                    Text("%s", string.c_str());
                    SameLine();
                    SetNextItemWidth(GetWindowWidth() - (max_size * width_scaler));
                    ColorEditGLMv3("##mat_color", &material->mColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB);

                    string = "SpecularSharpness:";
                    string.insert(string.end(), max_size - string.size(), ' ');
                    Text("%s", string.c_str());
                    SameLine();
                    SetNextItemWidth(GetWindowWidth() - (max_size * width_scaler));
                    InputInt("##SpecularSharpness", &material->mSpecularSharpness, 2, 8);

                    string = "SpecularStrength:";
                    string.insert(string.end(), max_size - string.size(), ' ');
                    Text("%s", string.c_str());
                    SameLine();
                    SetNextItemWidth(GetWindowWidth() - (max_size * width_scaler));
                    InputFloat("##SpecularStrength", &material->mSpecularStrength, 0.05f, 0.1f, "%.3f");

                    string = "DebugHighlight:";
                    string.insert(string.end(), max_size - string.size(), ' ');
                    Text("%s", string.c_str());
                    SameLine();
                    SetNextItemWidth(GetWindowWidth() - (max_size * width_scaler));
                    ColorEditGLMv4("##mat_DebugHighlight", &material->mDebugHighlight, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoAlpha);
                }
                EndChild();
            }
        }
        else
            { s_Thing = nullptr; }
    }
    End();
}
#endif // !DEBUGGING
