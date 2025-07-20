#ifndef PROTOTYPE_DEBUG_WINDOW_H
#define PROTOTYPE_DEBUG_WINDOW_H

#include "ui/imgui_object.hpp"

class prototype_DebugWindow : public ImGui_Object
{
public:
    bool Init();
    void Update();
    void Shutdown();
};

extern prototype_DebugWindow* global_DebugWindow;

#endif // PROTOTYPE_DEBUG_WINDOW_H
