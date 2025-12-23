#ifndef IMGUI_DEBUGGER_H
#define IMGUI_DEBUGGER_H

#define FWD_DCL
#   include "tools/stopwatch_log.hpp"
#undef  FWD_DCL

#include "ui/solution.hpp"

#include <string>

class ImGui_Debugger : public UI_Solution
{
public:
    Error Init()             final;
    void  Shutdown()         final;
    void  Update()           final;
    void  Input(InputEvent*) final;
    void  OnTheatreEntered() final;
    void  OnTheatreExited()  final;

    StopwatchLog& StartStopwatch(const std::string& Message = "No Message");
    bool StopStopwatch(StopwatchLog& Stopwatch);

    void StartTheatreTiming(bool IsTheatreLoading);
    void StopTheatreTiming(bool IsTheatreLoading);

    static void s_InspectTheatreWindow(bool* is_active);
private:
    StopwatchLog& m_StartStopwatch(const std::string& Message);
    bool m_StopStopwatch(StopwatchLog& Stopwatch);
};

extern ImGui_Debugger* g_pImGuiDebugger;

#endif // IMGUI_DEBUGGER_H
