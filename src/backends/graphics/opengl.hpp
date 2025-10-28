#ifndef OPENGL_BACKEND_H
#define OPENGL_BACKEND_H

#include "../graphics.hpp"
#include "rendering/shader_interfaces/gl_shader.hpp"

#include <map>

struct OpenGL_BufferData
{
    ID id{};
    std::vector<float> vertices{};
    std::vector<unsigned int> indices{};
};

struct OpenGL_MeshData
{
    uint indices_count{0};
    uint base_vertex{0};
    uint base_index{0};
};

class OpenGL_Backend : public GraphicsBackend
{
public:
    ~OpenGL_Backend() = default;

    bool Init();
    void Shutdown();
    const ID& GetID() const final { return BackendIDs::gOpenGL; }

    bool InitImGui() final;
    void ShutdownImGui() final;
    void ImGuiNewFrame() final;
    void ImGuiRender() final;

    void CreateRenderingData() final;
    void DestroyRenderingData() final;
    void BufferMesh(const FileData&, const ID&) final;
    void BufferTexture(const FileData&, const ID&) final;
    const ShaderInterface* GetShader(const ID&) const final;
    bool BuildShader(const ID&, const char*, const char*) final;
    bool BindShader(const ID&) final;
    bool DeleteShader(const ID&) final;
    void RenderSingleCommand(const RenderCommand&) final;
    void BufferLight(light_t*, unsigned int) final;

private:
#   define VAOS_AMOUNT 1
#   define VAO_DEFAULT 0
#   define VAO_DEFAULT_STRIDE 11

    std::array<uint, VAOS_AMOUNT> mVAOs{};
    std::map<ID, GLShader> mShaders{};

    std::map<ID, OpenGL_MeshData> mMeshData{};
    std::map<ID, uint> mTextureIDs{};

    uint GetTextureID(const ID&);
    OpenGL_MeshData* GetMeshData(const ID&);
    void UpdateViewport(const Viewport&) const final;
};

#ifdef DEBUGGING
    constexpr int Shader_ALL{0};
    constexpr int Shader_COLOR{1};
    constexpr int Shader_NORMAL{2};
    constexpr int Shader_UV{3};
    extern int g_ShaderDebugOuptut;
    inline bool g_EnableDebugMsgHigh{true};
    inline bool g_EnableDebugMsgMedium{true};
    inline bool g_EnableDebugMsgLow{true};
    inline bool g_EnableDebugMsgNotif{false};
#endif // DEBUGGING

#endif // OPENGL_BACKEND_H
