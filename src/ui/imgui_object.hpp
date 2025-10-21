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
    virtual void ReopenMainWindows() {}
    virtual void CloseAllWindows() = 0;
    virtual bool AreAllWindowsClosed() = 0;

protected:
    virtual ~ImGui_Object() {}
};

#endif // IMGUI_OBJECT_H
