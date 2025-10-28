#ifndef IMGUI_OBJECT_H
#define IMGUI_OBJECT_H

#include "input/fwd.hpp"

// Used by objects that derive `ImGui_Object`
#include <thirdparty/DearImGui/imgui.h>
#include <thirdparty/DearImGui/imgui_stdlib.h>

class ImGui_Object
{
public:
    virtual bool Init() { return true; }
    virtual void Shutdown() {}
    virtual void Update() {}
    virtual void TheatreStateChanged(bool IsStartup) {}
    virtual void Input(const InputEvent&) {}

protected:
    virtual ~ImGui_Object() {}
};

#endif // IMGUI_OBJECT_H
