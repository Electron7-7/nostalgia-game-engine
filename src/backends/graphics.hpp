#ifndef GRAPHICS_BACKEND_H
#define GRAPHICS_BACKEND_H

#include "rendering/fwd.hpp"
#include "things/fwd.hpp"
#include "filesystem/fwd.hpp"

#include "backend.hpp"

#include <glm/fwd.hpp>
#include <vector>

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

    virtual void DestroyRenderingData() = 0;
    virtual void CreateRenderingData() = 0;
    virtual void BufferMesh(const FileData& Data, const ID&) = 0;
    virtual void BufferTexture(const FileData& Data, const ID&) = 0;
    virtual void ClearBuffer(const glm::vec4& ClearColor) = 0;
    virtual const ShaderInterface* GetShader(unsigned int ShaderSelection) const = 0;
    virtual bool BindShader(unsigned int ShaderSelection) = 0;
    virtual bool BuildShader(unsigned int ShaderLabel, const char* VertexShaderCode, const char* FragmentShaderCode) = 0;
    virtual bool DeleteShader(unsigned int ShaderSelection) = 0;
    virtual void BufferLight(light_t* Light, unsigned int ShaderLabel) = 0;
    virtual void RenderSingleCommand(const RenderCommand& RenderCommand) = 0;

protected:
    unsigned int mCurrentlyBoundShader{0};

    static bool CreateMeshData(std::vector<float>& VertexData, std::vector<unsigned int>& Indices, const FileData& FileData);
};

#endif // GRAPHICS_BACKEND_H
