#ifndef BACKENDS_H
#define BACKENDS_H

#include "rendering/render_command.hpp"
#include "rendering/shader_interfaces/shader_interface.hpp"
#include "safe_return.hpp"

#include <glm/glm.hpp>
#include <set>

typedef int BackendID;

struct BackendIDs
{
    static constexpr BackendID gOpenGL = 0;
    static constexpr BackendID wGLFW   = 0;

    static constexpr BackendID INVALID = -1;
};

class _Backend
{
public:
    virtual bool Init() = 0;
    virtual void Shutdown() = 0;
    virtual BackendID GetID() = 0;

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
    virtual void Shutdown() { is_initialized = false; is_imgui_initialized = false; is_theatre_data_initialized = false; }
    virtual BackendID GetID() { return BackendIDs::INVALID; }

    virtual bool InitImGui() { return true; }
    virtual void ImGuiNewFrame() {}
    virtual void ImGuiRender() {}

    virtual bool InitNewTheatre() = 0;
    virtual void ClearBuffer(glm::vec4 ClearColor) = 0;
    virtual const ShaderInterface* GetShader(unsigned int ShaderSelection) const = 0;
    virtual bool BindShader(unsigned int ShaderSelection) = 0;
    virtual bool BuildShader(unsigned int ShaderLabel, const std::string& VertexShaderCode, const std::string& FragmentShaderCode) = 0;
    virtual bool RebuildShader(unsigned int ShaderLabel, const std::string& VertexShaderCode, const std::string& FragmentShaderCode) = 0;
    virtual void RenderSingleCommand(const RenderCommand& RenderCommand) = 0;

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
    virtual void Shutdown() { is_initialized = false; is_imgui_initialized = false; }
    virtual BackendID GetID() { return BackendIDs::INVALID; }

    virtual bool InitImGui() { return true; }
    virtual void ImGuiNewFrame() {}

    virtual SafeStatus CreateMainWindow() = 0;

    virtual void ResizeWindow(int Width, int Height) = 0;
    virtual void MoveWindow(int XPosition, int YPosition) = 0;
    virtual void SetFullscreen(bool FullscreenOn) = 0;

    virtual void SwapBuffers() = 0;
    virtual void PollEvents() = 0;
    virtual void UpdateState() = 0;

    bool CompatibleWith(BackendID GraphicsBackend) const { return compatible_graphics_backends.contains(GraphicsBackend); }

protected:
    std::set<BackendID> compatible_graphics_backends = {};
    bool is_imgui_initialized = false;
};

#endif // BACKENDS_H
