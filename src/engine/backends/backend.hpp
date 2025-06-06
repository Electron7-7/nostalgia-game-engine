#ifndef BACKENDS_H
#define BACKENDS_H

#include <glm/glm.hpp>

class _Backend
{
public:
    virtual bool Init() = 0;
    virtual void Shutdown() = 0;

protected:
    virtual ~_Backend() = default;

    bool is_initialized = false;
};

class GraphicsBackend : public _Backend
{
public:
    virtual ~GraphicsBackend() = default;

    virtual bool Init() { return true; }
    virtual void Shutdown() {}

    // Prototype Functions
    virtual void prototype_ClearBuffer(glm::vec4 ClearColor) {}
    virtual void prototype_RenderFrame() {}
};

class WindowingBackend : public _Backend
{
public:
    virtual ~WindowingBackend() = default;

    virtual bool Init() { return true; }
    virtual void Shutdown() {}
    virtual bool CreateMainWindow() { return true; }

    // Prototype Functions
    virtual bool prototype_SetFullscreen(bool FullscreenOn) { return true; }
    virtual void prototype_SwapBuffers() {}
    virtual void prototype_PollEvents() {}
};

namespace BackendIDs
{
    constexpr int OpenGL_GLFW = 0;

    constexpr int Default = OpenGL_GLFW;
}
protected:
    friend class RenderManager;
    friend class BackendManager;
    inline static int main_WindowPositionX = 0;
    inline static int main_WindowPositionY = 0;
    inline static int main_WindowWidth = 1280;
    inline static int main_WindowHeight = 720;
    inline static bool main_WindowIsFullscreen = false;
    inline static bool main_WindowIsCentered = true;
    inline static const char* main_WindowName = "Nostalgia";

#endif // BACKENDS_H