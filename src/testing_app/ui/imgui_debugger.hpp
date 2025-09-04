#ifndef IMGUI_DEBUGGER_H
#define IMGUI_DEBUGGER_H

#include "ui/imgui_object.hpp"

class imgui_Debugger : public ImGui_Object
{
public:
    bool Init();
    void Shutdown();
    void Update();
};

extern bool   g_TrackingStart;
extern bool   g_TrackingStop;
extern double g_TrackingStartTime;
extern double g_TrackingStopTime;

extern imgui_Debugger* g_pDebugger;

#endif // IMGUI_DEBUGGER_H
