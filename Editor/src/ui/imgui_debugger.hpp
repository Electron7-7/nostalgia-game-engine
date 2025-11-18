#ifndef IMGUI_DEBUGGER_H
#define IMGUI_DEBUGGER_H

#include "user_interface/imgui_object.hpp"
#include "tools/fwd.hpp"

#include <string>

class imgui_Debugger : public ImGui_Object
{
public:
    void Update();

    StopwatchLog& StartStopwatch(const std::string& Message = "No Message");
    bool StopStopwatch(StopwatchLog& Stopwatch);

    void StartTheatreTiming(bool IsTheatreLoading);
    void StopTheatreTiming(bool IsTheatreLoading);

    static void s_InspectTheatreWindow(bool* is_active);
private:
    StopwatchLog& m_StartStopwatch(const std::string& Message);
    bool m_StopStopwatch(StopwatchLog& Stopwatch);
};

extern imgui_Debugger* g_pDebugger;

#endif // IMGUI_DEBUGGER_H
