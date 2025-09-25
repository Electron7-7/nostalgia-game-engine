#ifndef GRAPHICS_BACKEND_H
#define GRAPHICS_BACKEND_H

#include "rendering/fwd.hpp"
#include "things/fwd.hpp"
#include "filesystem/fwd.hpp"

#include "backend.hpp"

#include <glm/glm.hpp>
#include <vector>

class GraphicsBackend : public _Backend
{
public:
    // Inherited by _Backend
    virtual ~GraphicsBackend() = default;
    virtual bool Init() { return true; }
    virtual void Shutdown() { m_IsInitialized = false; m_IsImGuiInitialized = false; }
    virtual BackendID GetID() { return gBackendIDs::Invalid; }

    // Virtual functions (can be unimplemented)
    virtual bool InitImGui() { return true; }
    virtual void ImGuiNewFrame() {}
    virtual void ImGuiRender() {}

    // Pure virtual functions (*must* be implemented)
    virtual void DestroyRenderingData() = 0;
    virtual void CreateRenderingData() = 0;
    virtual void BufferMesh(const FileData& Data, id_t ID) = 0;
    virtual void BufferTexture(const FileData& Data, id_t ID) = 0;
    virtual void ClearBuffer(glm::vec4 ClearColor) = 0;
    virtual const ShaderInterface* GetShader(unsigned int ShaderSelection) const = 0;
    virtual bool BindShader(unsigned int ShaderSelection) = 0;
    virtual bool BuildShader(unsigned int ShaderLabel, const char* VertexShaderCode, const char* FragmentShaderCode) = 0;
    virtual bool DeleteShader(unsigned int ShaderSelection) = 0;
    virtual void BufferLight(light_t* Light, unsigned int ShaderLabel) = 0;
    virtual void RenderSingleCommand(const RenderCommand& RenderCommand) = 0;

protected:
    bool m_IsImGuiInitialized = false;
    static unsigned int s_CurrentlyBoundShader;

    static bool CreateMeshData(std::vector<float>& VertexData, std::vector<unsigned int>& Indices, const FileData& FileData);
};

#endif // GRAPHICS_BACKEND_H
