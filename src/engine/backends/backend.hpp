#ifndef BACKENDS_H
#define BACKENDS_H

#include "engine/rendering/shader_interfaces/shader_interface.hpp"
#include <glm/glm.hpp>
#include <set>

// This typedef keeps the code self-documenting
typedef int BackendID;
// These typedefs help make the code easier to understand in certain areas (so, more self-documentation)
typedef int GraphicsBackendID;
typedef int WindowingBackendID;

namespace BackendIDs
{
    constexpr GraphicsBackendID OpenGL = 0;
    constexpr WindowingBackendID GLFW  = 0;

    constexpr BackendID default_Graphics = OpenGL;
    constexpr BackendID default_Windowing = GLFW;
}

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
    // Inherited by _Backend
    virtual ~GraphicsBackend() = default;
    virtual bool Init() { return true; }
    virtual void Shutdown() {}

    virtual const ShaderInterface* GetShader(unsigned int ShaderSelection) const = 0;
    virtual bool BuildShader(unsigned int ShaderLabel, const std::string& VertexShaderCode, const std::string& FragmentShaderCode) = 0;
    virtual bool RebuildShader(unsigned int ShaderLabel, const std::string& VertexShaderCode, const std::string& FragmentShaderCode) = 0;

    // Prototype Functions
    virtual void prototype_ClearBuffer(glm::vec4 ClearColor) {}
};

class WindowingBackend : public _Backend
{
public:
    // Inherited by _Backend
    virtual ~WindowingBackend() = default;
    virtual bool Init() { return true; }
    virtual void Shutdown() {}

    virtual bool CreateMainWindow() = 0;

    // Prototype Functions
    virtual bool prototype_SetFullscreen(bool FullscreenOn) = 0;
    virtual void prototype_SwapBuffers() = 0;
    virtual void prototype_PollEvents() = 0;

    // Compatibility Functions
    bool CompatibleWith(GraphicsBackendID GraphicsBackend) const { return compatible_graphics_ids.contains(GraphicsBackend); }
    std::set<GraphicsBackendID> GetCompatibleGraphicsIDs() const { return compatible_graphics_ids; }

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

    std::set<BackendID> compatible_graphics_ids = {};
};

#endif // BACKENDS_H
