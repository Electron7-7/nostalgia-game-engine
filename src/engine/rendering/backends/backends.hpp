#ifndef BACKENDS_H
#define BACKENDS_H

#include "engine/rendering/render_context.hpp"

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

protected:
    friend class RenderManager;
};

class WindowingBackend : public _Backend
{
public:
    virtual ~WindowingBackend() = default;

    virtual bool Init() { return true; }
    virtual void Shutdown() {}
    virtual bool CreateMainWindow() { return true; }

    // Prototype Functions
    virtual void prototype_SwapBuffers() {}
    virtual void prototype_PollEvents() {}

protected:
    friend class RenderManager;

    bool center_aligned_if_windowed = true;
};

namespace BackendIDs
{
    constexpr int OpenGL_GLFW = 0;

    constexpr int Default = OpenGL_GLFW;
}

#endif // BACKENDS_H