#ifndef OPENGL_BACKEND_H
#define OPENGL_BACKEND_H

#include "../../backends/backend.hpp"
#include "../../shader_interfaces/gl_shader.hpp"

#include <map>
#include <vector>

typedef unsigned int OpenGL_TextureID;

struct OpenGL_MeshData
{
    OpenGL_MeshData(const std::vector<float>& data = {}, const std::vector<unsigned int>& indices_vec = {})
    : vertex_data(data), indices(indices_vec), indices_count(indices_vec.size()), base_vertex(0), base_index(0)
    {}

    std::vector<float> vertex_data = {};
    std::vector<unsigned int> indices = {};

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
    void ClearBuffer(glm::vec4);
    const ShaderInterface* GetShader(unsigned int) const;
    bool BuildShader(unsigned int, const char*, const char*);
    bool BindShader(unsigned int);
    bool DeleteShader(unsigned int);
    void RenderSingleCommand(const RenderCommand&);

private:
#   define VAOS_AMOUNT 1
#   define VAO_DEFAULT 0
#   define VAO_DEFAULT_STRIDE 11

    static std::array<unsigned int, VAOS_AMOUNT> m_VAOs;
    static std::map<unsigned int, GLShader> m_Shaders;

    static std::map<id_t, OpenGL_MeshData> m_MeshData;
    static std::map<id_t, OpenGL_TextureID> m_TextureIDs;

    static unsigned int GetTextureID(id_t);
};

extern OpenGL_Backend singleton_OpenGL_Backend;

#ifdef DEBUGGING
    constexpr int Shader_ALL = 0;
    constexpr int Shader_COLOR = 1;
    constexpr int Shader_NORMAL = 2;
    constexpr int Shader_UV = 3;
    extern int g_ShaderDebugOuptut;
#endif // DEBUGGING

#endif // OPENGL_BACKEND_H
