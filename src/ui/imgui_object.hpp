#ifndef IMGUI_OBJECT_H
#define IMGUI_OBJECT_H

#include "input/fwd.hpp"

class ImGui_Object
{
public:
    virtual bool Init() { return true; }
    virtual void Shutdown() {}
    virtual void Update() {}
    virtual void Input(const InputEvent&) {}

protected:
    virtual ~ImGui_Object() {}
};

#endif // IMGUI_OBJECT_H
