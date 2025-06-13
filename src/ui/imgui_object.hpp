#ifndef IMGUI_OBJECT_H
#define IMGUI_OBJECT_H

class ImGui_Object
{
public:
    virtual bool Init() = 0;
    virtual void Update() = 0;
    virtual void Shutdown() = 0;

protected:
    virtual ~ImGui_Object() {}

    bool is_initialized = false;
    bool shutdown_requested = false;
};

#endif // IMGUI_OBJECT_H
