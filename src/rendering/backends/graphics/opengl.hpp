#ifndef OPENGL_BACKEND_H
#define OPENGL_BACKEND_H

#include "rendering/backends/backend.hpp"
#include "rendering/render_command.hpp"
#include "rendering/shader_interfaces/gl_shader.hpp"

#include <map>
#include <array>
#include <vector>

typedef unsigned int OpenGL_TextureID;

struct OpenGL_MeshData
{
    std::vector<float>        vertex_data = {};
    std::vector<unsigned int> indices     = {};

    unsigned int indices_count = 0;
    unsigned int base_vertex   = 0;
    unsigned int base_index    = 0;
};

class OpenGL_Backend : public GraphicsBackend
{
public:
    ~OpenGL_Backend() = default;

    bool Init();
    void Shutdown();
    BackendID GetID() { return BackendIDs::gOpenGL; }

    bool InitImGui();
    void ImGuiNewFrame();
    void ImGuiRender();

    void CreateRenderingData();
    void DestroyRenderingData();
    void BufferMesh(Mesh*);
    void BufferTexture(Texture*);
    void ClearBuffer(glm::vec4 ClearColor);
    const ShaderInterface* GetShader(unsigned int ShaderSelection) const;
    bool BindShader(unsigned int ShaderLabel);
    bool BuildShader(unsigned int ShaderLabel, const char* VertexShaderCode, const char* FragmentShaderCode);

    void RenderSingleCommand(const RenderCommand& RenderCommand);

private:
#   define VAOS_AMOUNT 1
#   define VAO_DEFAULT 0
#   define VAO_DEFAULT_STRIDE 11

    static std::array<unsigned int, VAOS_AMOUNT> m_VAOs;
    static std::map<unsigned int, GLShader> m_Shaders;

    static std::map<id_t, OpenGL_MeshData> m_MeshData;
    static std::map<id_t, OpenGL_TextureID> m_TextureIDs;

    static unsigned int m_CurrentlyBoundShader;

    static unsigned int GetTextureID(id_t);
};

extern OpenGL_Backend singleton_OpenGL_Backend;

#endif // OPENGL_BACKEND_H
