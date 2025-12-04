#ifndef IMGUI_DEBUGGER_H
#define IMGUI_DEBUGGER_H

#include "tools/fwd.hpp"

#include "ui/solution.hpp"
#include "backends/imgui/imgui_implementor.hpp"

#include <string>

class ImGui_Debugger : public UI_Solution<ImGui_Implementor>
{
    UI_SOLUTION_CONSTRUCTOR(ImGui_Debugger, ImGui_Implementor)
    UI_SOLUTION_INSTANCE(ImGui_Debugger)
    UI_SOLUTION_ACTIVATE(ImGui_Debugger)
    UI_SOLUTION_DEACTIVATE(ImGui_Debugger)
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

#endif // IMGUI_DEBUGGER_H
