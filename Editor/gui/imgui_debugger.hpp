#ifndef IMGUI_DEBUGGER_H
#define IMGUI_DEBUGGER_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/ui/solution.hpp>
#include "tools/stopwatch_log.hpp"

class ImGui_Debugger : public UI_Solution
{
public:
    inline static bool m_sDebugConsoleOpened{false}, m_sDebugWindowOpened{false};

    void Init()             final;
    void Shutdown()         final;
    void Update()           final;
    void Input(InputEvent*) final;
    void TheatreEntered() final;
    void TheatreExited()  final;

    StopwatchLog& StartStopwatch(Sarg Message = "No Message");
    bool StopStopwatch(StopwatchLog& Stopwatch);

    void StartTheatreTiming(bool IsTheatreLoading);
    void StopTheatreTiming(bool IsTheatreLoading);

    void DebugConsoleWindow();

private:
    RMutex mStopwatchMutex{};

    StopwatchLog& m_StartStopwatch(Sarg Message);
    bool m_StopStopwatch(StopwatchLog& Stopwatch);
    void m_AutomaticStopwatchWindow(float inWidth);
    void m_ManualStopwatchWindow(float inWidth);
};

extern ImGui_Debugger* g_pImGuiDebugger;

#endif // IMGUI_DEBUGGER_H
