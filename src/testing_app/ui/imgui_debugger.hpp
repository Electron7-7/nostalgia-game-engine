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

extern imgui_Debugger* global_Debugger;

#endif // IMGUI_DEBUGGER_H
