#include "imgui_debugger.hpp"
#include "managers/theatre_manager.hpp"
#include "time.hpp"
#include "managers/backend_manager.hpp"
#include "theatre/theatre_parser.hpp"
#include "settings/settings.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include "thirdparty/DearImGui/imgui_stdlib.h"

#include <format>

using namespace ImGui;

bool   g_TrackingStart = false;
bool   g_TrackingStop  = false;
double g_TrackingStartTime = 0.0;
double g_TrackingStopTime  = 0.0;

static imgui_Debugger s_Debugger;
imgui_Debugger* g_pDebugger = &s_Debugger;

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
    static std::string s_TheatreFilePath = "theatres/NostalgiaHelloWorld.nt";
    static bool loading_theatre = false;
    static double theatre_load_start_time = 0.0;
    static double theatre_load_stop_time  = 0.0;
    static double theatre_load_time_spent = 0.0;
    static bool unloading_theatre = false;
    static double theatre_unload_start_time = 0.0;
    static double theatre_unload_stop_time  = 0.0;
    static double theatre_unload_time_spent = 0.0;

    static bool   update_stopwatch_visual = false;
    static double tracking_total_time = 0.0;
    static bool   auto_stopwatch_enabled = true;

    static std::vector<std::pair<std::string, double>> theatre_load_times = {};
    static std::vector<std::pair<std::string, double>> theatre_unload_times = {};
    static std::vector<std::tuple<double, double, double>> stopwatch_logs = {};

    Begin("Stopwatch Variables");
    if(Button("Start Stopwatch"))
        { g_TrackingStart = true; }
    if(Button("Stop Stopwatch"))
        { g_TrackingStop = true; }
    if(Button("Clear Stopwatch Logs"))
        { stopwatch_logs.clear(); }
    std::string stopwatch_toggle_label = std::format("Toggle Automatic Stopwatch (Currently {})", (auto_stopwatch_enabled) ? "On" : "Off");
    const char* stopwatch_toggle_label_cstr = stopwatch_toggle_label.c_str();
    if(Button(stopwatch_toggle_label_cstr))
        { auto_stopwatch_enabled = !auto_stopwatch_enabled; }
    if(IsItemHovered())
        { SetTooltip("%s", "This enables/disables the program's ability to start/stop the Stopwatch"); }

    if(update_stopwatch_visual)
        { Text("Current Stopwatch: %f", Time::Current() - g_TrackingStartTime); }
    else
        { Text("Current Stopwatch: Stopped"); }

    Text("Stopwatch Logs:");
    for(const auto& [start, stop, total] : stopwatch_logs)
    {
        Separator();
        Text("Time: %f", total);
        Text("Started @ %f, Stopped @ %f", start, stop);
    }
    End();

    if(g_TrackingStart)
    {
        g_TrackingStart = false;
        update_stopwatch_visual = true;
        g_TrackingStartTime = Time::Current();
    }

    if(g_TrackingStop)
    {
        g_TrackingStop = false;
        update_stopwatch_visual = false;
        g_TrackingStopTime = Time::Current();
        tracking_total_time = g_TrackingStopTime - g_TrackingStartTime;
        stopwatch_logs.push_back({g_TrackingStartTime, g_TrackingStopTime, tracking_total_time});
    }

    Begin("Debug Window");

#ifdef DEBUGGING
    if(Button("Toggle Frame Number Printouts"))
        { g_PrintFrameNumbers = !g_PrintFrameNumbers; }
    if(Button("Toggle Tick Number Printouts"))
        { g_PrintTickNumbers = !g_PrintTickNumbers; }
    InputInt("Parser Breakpoint Line#", &g_BreakOnLine);
    InputInt("Parser Breakpoint Column#", &g_BreakOnColumn);
#endif // DEBUGGING
    InputText("NostalgiaTheatre File Path", &s_TheatreFilePath);
    if(loading_theatre && _Manager::GetTheatreState() == ManagerEnums::TheatreState_t::IN_LEVEL)
    {
        loading_theatre = false;
        theatre_load_stop_time = Time::Current();
        theatre_load_time_spent = theatre_load_stop_time - theatre_load_start_time;
        theatre_load_times.push_back({s_TheatreFilePath, theatre_load_time_spent});
    }
    if(unloading_theatre && _Manager::GetTheatreState() == ManagerEnums::TheatreState_t::NOT_IN_LEVEL)
    {
        unloading_theatre = false;
        theatre_unload_stop_time = Time::Current();
        theatre_unload_time_spent = theatre_unload_stop_time - theatre_unload_start_time;
        theatre_unload_times.push_back({s_TheatreFilePath, theatre_unload_time_spent});
    }

    if(Button("Load Theatre"))
    {
        if(_Manager::GetTheatreState() == ManagerEnums::TheatreState_t::NOT_IN_LEVEL)
        {
            loading_theatre = true;
            theatre_load_start_time = Time::Current();
            if(SafeStatus::PrintCheck(TheatreParser::try_LoadTheatreFromFile(s_TheatreFilePath)))
                { _Manager::StartNewTheatre(); }
        }
    }

    if(Button("Exit Test Theatre"))
    {
        if(_Manager::GetTheatreState() == ManagerEnums::TheatreState_t::IN_LEVEL)
        {
            unloading_theatre = true;
            theatre_unload_start_time = Time::Current();
            _Manager::ShutdownTheatre();
        }
    }

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

    End();

    Begin("Theatre Load/Unload Times");
    if(Button("Clear Theatre Timing Logs"))
    {
        theatre_load_times.clear();
        theatre_unload_times.clear();
    }
    Separator();
    Text("Load Times:");
    for(size_t i = 0 ; i < theatre_load_times.size() ; ++i)
    {
        Text("#%zu (%s):\nLoad Time: %fs", i+1, theatre_load_times.at(i).first.c_str(), theatre_load_times.at(i).second);
        if(i < theatre_unload_times.size())
            { Text("Unload Time: %fs", theatre_unload_times.at(i).second); }
    }

    End();
}
