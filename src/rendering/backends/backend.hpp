#ifndef BACKENDS_H
#define BACKENDS_H

#include "rendering/render_command.hpp"
#include "rendering/shader_interfaces/shader_interface.hpp"
#include "safe_return.hpp"

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
    virtual bool InitImGui() { return true; }
    virtual bool InitNewTheatre() { return true; }
    virtual void Shutdown() {}
    virtual void ImGuiNewFrame() {}
    virtual void ImGuiRender() {}

    virtual const ShaderInterface* GetShader(unsigned int ShaderSelection) const = 0;
    virtual bool BindShader(unsigned int ShaderSelection) = 0;
    virtual bool BuildShader(unsigned int ShaderLabel, const std::string& VertexShaderCode, const std::string& FragmentShaderCode) = 0;
    virtual bool RebuildShader(unsigned int ShaderLabel, const std::string& VertexShaderCode, const std::string& FragmentShaderCode) = 0;
    virtual void RenderSingleCommand(const RenderCommand& RenderCommand) = 0;

    // Prototype Functions
    virtual void prototype_ClearBuffer(glm::vec4 ClearColor) = 0;

protected:
    bool is_imgui_initialized = false;
    bool is_theatre_data_initialized = false;
};

class WindowingBackend : public _Backend
{
public:
    // Inherited by _Backend
    virtual ~WindowingBackend() = default;
    virtual bool Init() { return true; }
    virtual bool InitImGui() { return true; }
    virtual void Shutdown() {}
    virtual void ImGuiNewFrame() {}

    virtual bool CreateMainWindow() = 0;
    virtual SafeReturn<size_t> CreateWindow(const char* WindowName) = 0;

    virtual void SwapBuffers() = 0;
    virtual void PollEvents() = 0;

    // Prototype Functions
    virtual bool prototype_SetFullscreen(bool FullscreenOn) = 0;

    // Compatibility Functions
    bool CompatibleWith(GraphicsBackendID GraphicsBackend) const { return compatible_graphics_ids.contains(GraphicsBackend); }
    std::set<GraphicsBackendID> GetCompatibleGraphicsIDs() const { return compatible_graphics_ids; }

protected:
    std::set<BackendID> compatible_graphics_ids = {};
    bool is_imgui_initialized = false;
};

#endif // BACKENDS_H
