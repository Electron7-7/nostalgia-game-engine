#ifndef OPENGL_BACKEND_H
#define OPENGL_BACKEND_H

#include "graphics.hpp"
#include "rendering/shader_interfaces/gl_shader.hpp"

#include <map>

typedef unsigned int OpenGL_TextureID;

struct OpenGL_BufferData
{
    id_t id = 0;
    std::vector<float> vertices = {};
    std::vector<unsigned int> indices = {};
};

struct OpenGL_MeshData
{
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
    BackendID GetID() final
    { return gBackendIDs::gOpenGL; }

    bool InitImGui();
    void ImGuiNewFrame();
    void ImGuiRender();

    void CreateRenderingData();
    void DestroyRenderingData();
    void BufferMesh(const FileData&, id_t);
    void BufferTexture(const FileData&, id_t);
    void ClearBuffer(glm::vec4);
    const ShaderInterface* GetShader(unsigned int) const;
    bool BuildShader(unsigned int, const char*, const char*);
    bool BindShader(unsigned int);
    bool DeleteShader(unsigned int);
    void RenderSingleCommand(const RenderCommand&);
    void BufferLight(light_t*, unsigned int);

private:
#   define VAOS_AMOUNT 1
#   define VAO_DEFAULT 0
#   define VAO_DEFAULT_STRIDE 11

    static std::array<unsigned int, VAOS_AMOUNT> m_VAOs;
    static std::map<unsigned int, GLShader> m_Shaders;

    static std::map<id_t, OpenGL_MeshData> m_MeshData;
    static std::map<id_t, OpenGL_TextureID> m_TextureIDs;

    unsigned int GetTextureID(id_t);
    OpenGL_MeshData* GetMeshData(id_t);
};

#ifdef DEBUGGING
    constexpr int Shader_ALL = 0;
    constexpr int Shader_COLOR = 1;
    constexpr int Shader_NORMAL = 2;
    constexpr int Shader_UV = 3;
    extern int g_ShaderDebugOuptut;
    inline bool g_EnableDebugMsgHigh = true;
    inline bool g_EnableDebugMsgMedium = true;
    inline bool g_EnableDebugMsgLow = true;
    inline bool g_EnableDebugMsgNotif = false;
#endif // DEBUGGING

#endif // OPENGL_BACKEND_H
