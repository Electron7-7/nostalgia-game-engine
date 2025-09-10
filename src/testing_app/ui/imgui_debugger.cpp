#include "imgui_debugger.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/DearImGui/imgui_stdlib.h"
#include "things/resources/complex/mesh_instance.hpp"
#include "things/actors/prototype_actor.hpp"

using namespace ImGui;

static imgui_Debugger s_Debugger;
imgui_Debugger* g_pDebugger = &s_Debugger;

#ifndef DEBUGGING
void imgui_Debugger::Update()
{
    static bool show_demo_window = false;
    if(show_demo_window)
        { ShowDemoWindow(&show_demo_window); }

    if(Begin("Debugging", nullptr, ImGuiWindowFlags_MenuBar))
    {
        if(BeginMenuBar())
        {
            if(BeginMenu("ImGui"))
            {
                if(MenuItem("Show ImGui Demo Window", "CTRL+G"))
                    { show_demo_window = true; }
                EndMenu();
            }
            EndMenuBar();
            Text("%s", "You are currently testing the 'Release' version of Nostalgia.\nIf you want to use the debug window, please compile the 'Debug' version of both Nostalgia and the included testing app.\nThis can be done with 'make debug static testapp_static'.");
        }
    }
    End();
}
#else  // DEBUGGING
#include "time.hpp"
#include "managers/backend_manager.hpp"
#include "theatre/theatre_parser.hpp"
#include "managers/theatre_manager.hpp"
#include "settings/settings.hpp"

#include <set>
#include <random>

static std::random_device s_RandomDevice;

static std::string s_TheatreFilePath = "theatres/NostalgiaHelloWorld.nt";
static std::string s_LastAttemptedTheatreFilePath = s_TheatreFilePath;

static bool s_AutoStopwatchEnabled = true;

static std::vector<TheatreLog>   s_TheatreLogs = {};
static std::vector<StopwatchLog> s_ManualStopwatchLogs = {};
static std::vector<StopwatchLog> s_StopwatchLogs = {};
static std::set<int>             s_StopwatchLogIds = {};

static void s_AutomaticStopwatchWindow(float);
static void s_ManualStopwatchWindow(float);
static void s_GeneralDebuggingWindow();
static void s_TheatreDebuggingWindow();
static void s_InspectTheatreWindow(bool* is_active);

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

    m_FormattedStartTime = Time::CurrentSystem();
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
    m_FormattedStopTime = Time::CurrentSystem();
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
    // TODO: Make this thread safe
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

static void s_GeneralDebuggingWindow()
{
    BeginChild("General Debugging");
    if(Button("Toggle Frame Number Printouts"))
        { g_PrintFrameNumbers = !g_PrintFrameNumbers; }
    if(Button("Toggle Tick Number Printouts"))
        { g_PrintTickNumbers = !g_PrintTickNumbers; }

    Text("%s", "Settings");
    Separator();

    InputInt("X Position", &Settings::Window::XPosition);
    InputInt("Y Position", &Settings::Window::YPosition);
    InputInt("Width", &Settings::Window::Width);
    InputInt("Height", &Settings::Window::Height);
    InputInt("Fullscreen Width", &Settings::Window::FullscreenWidth);
    InputInt("Fullscreen Height", &Settings::Window::FullscreenHeight);

    Separator();
    Checkbox("Fullscreen", &Settings::Window::Fullscreen);
    if(Button("Apply Changes"))
        { g_pBackendManager->UpdateWindowState(); }
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
        if(SafeStatus::PrintCheck(TheatreParser::try_LoadTheatreFromFile(s_TheatreFilePath)))
            { _Manager::StartNewTheatre(); }
    }
    if(_Manager::GetTheatreState() != ManagerEnums::NOT_IN_LEVEL)
        { EndDisabled(); }

    if(_Manager::GetTheatreState() != ManagerEnums::IN_LEVEL)
        { BeginDisabled(); }
    if(Button("Exit Theatre"))
        { _Manager::ShutdownTheatre(); }
    if(_Manager::GetTheatreState() != ManagerEnums::IN_LEVEL)
        { EndDisabled(); }

    static bool s_TheatreInspectorActive = false;
    bool not_in_level = (_Manager::GetTheatreState() == ManagerEnums::NOT_IN_LEVEL);

    if(not_in_level)
    {
        BeginDisabled();
        s_TheatreInspectorActive = false;
    }
    if(Button("Inspect Theatre"))
        { s_TheatreInspectorActive = !s_TheatreInspectorActive; }

    if(s_TheatreInspectorActive)
        { s_InspectTheatreWindow(&s_TheatreInspectorActive); }
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

void s_InspectTheatreWindow(bool* is_active)
{
    if(Begin("Theatre Inspector", is_active))
    {
        const std::map<id_t, std::shared_ptr<Thing>>& things = TheatreManager::debug_GetThings();
        for(const auto& [id, thing] : things)
        {
            Button(thing->GetName().c_str(), {0.0f, 20.0f});
            if(IsItemHovered())
            {
                BeginTooltip();
                    switch(thing->GetType())
                    {
                    case Type::PrototypeActor:
                        Text("MeshInstance: %u", dynamic_pointer_cast<PrototypeActor>(thing)->GetMeshInstance());
                        break;
                    case Type::MeshInstance:
                        Text("Mesh: %u", dynamic_pointer_cast<MeshInstance>(thing)->GetMesh());
                        Text("Material: %u", dynamic_pointer_cast<MeshInstance>(thing)->GetMaterial());
                        break;
                    default:
                        break;
                    }
                    Text("Name: %s", thing->GetName().c_str());
                    Text("ID: %u", thing->GetID());
                    Text("TypeName: %s", StringifyType(thing->GetType()));
                EndTooltip();
            }
        }
    }
    End();
}

void imgui_Debugger::Update()
{
    static bool show_demo_window = false;
    if(show_demo_window)
        { ShowDemoWindow(&show_demo_window); }

    static bool s_PopOutStopwatches = false;

    SetNextWindowSize({840,480}, ImGuiCond_FirstUseEver);
    if(Begin("Debugging", nullptr, ImGuiWindowFlags_MenuBar))
    {
        if(BeginMenuBar())
        {
            if(BeginMenu("ImGui"))
            {
                if(MenuItem("Show ImGui Demo Window", "CTRL+G"))
                    { show_demo_window = true; }
                EndMenu();
            }
            EndMenuBar();
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
    }
    End();
}
#endif // !DEBUGGING
