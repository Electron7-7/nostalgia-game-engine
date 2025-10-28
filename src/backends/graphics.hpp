#ifndef GRAPHICS_BACKEND_H
#define GRAPHICS_BACKEND_H

#include "rendering/fwd.hpp"
#include "things/fwd.hpp"
#include "filesystem/fwd.hpp"

#include "backend.hpp"
#include "rendering/viewport.hpp"
#include "rendering/render_command.hpp"
#include "settings/window.hpp"

#include <glm/fwd.hpp>
#include <vector>
#include <map>

class GraphicsBackend : public _Backend
{
public:
    virtual ~GraphicsBackend() = default;
    virtual bool Init() { return true; }
    virtual void Shutdown() {}
    virtual const ID& GetID() const { return BackendIDs::Invalid; }

    virtual bool InitImGui() { return false; }
    virtual void ShutdownImGui() {}
    virtual void ImGuiNewFrame() {}
    virtual void ImGuiRender() {}

    void BufferRenderCommand(const RenderCommand&);
    void RenderStoredCommands();

    bool UseViewport(const ID& ViewportID);
    const ID& CurrentViewportID();
    const Viewport& CurrentViewport();
    const Viewport& WindowViewport();
    const Viewport& GetViewport(const ID& ViewportID);
    bool SetViewport(const ID& ViewportID, const Viewport& outViewport);
    void SetWindowViewport(const Viewport& outViewport);
    bool SetCurrentViewport(const Viewport& outViewport);
    bool IsViewportAt(const ID& ViewportID);
    uint PushViewport(const Viewport& Viewport);
    bool PopViewport(const ID& ID);

    virtual const ShaderInterface* GetShader(const ID& ShaderSelection) const = 0;
    virtual bool BindShader(const ID& ShaderSelection) = 0;
    virtual bool BuildShader(const ID& ShaderLabel, const char* VertexShaderCode, const char* FragmentShaderCode) = 0;
    virtual bool DeleteShader(const ID& ShaderSelection) = 0;

    virtual void RenderSingleCommand(const RenderCommand& RenderCommand) = 0;
    virtual void DestroyRenderingData() = 0;
    virtual void CreateRenderingData() = 0;
    virtual void BufferMesh(const FileData& Data, const ID&) = 0;
    virtual void BufferTexture(const FileData& Data, const ID&) = 0;
    virtual void SetLight(light_t* Light, const ID& ShaderLabel) = 0;

protected:
    std::vector<RenderCommand> mRenderCommands{};

    ID mCurrentlyBoundShader{Shaders::BlinnPhong};
    ID mCurrentViewport{ViewportIDs::Window};

    std::map<ID, Viewport> mViewports{
        { ViewportIDs::Window, Viewport{Settings::Window::FramebufferSize(), Settings::Window::FramebufferPosition()} },
        { ViewportIDs::Editor3DViewport1, Viewport{{500, 500}} }
    };

    virtual void UpdateViewport(const Viewport&) const = 0;

    static bool CreateMeshData(std::vector<float>& VertexData, std::vector<unsigned int>& Indices, const FileData& FileData);

private:
    ID mLastUsedViewport{};
};

#endif // GRAPHICS_BACKEND_H
