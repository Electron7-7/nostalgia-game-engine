#ifndef OPENGL_BACKEND_H
#define OPENGL_BACKEND_H

#include "rendering/backends/backend.hpp"
#include "rendering/render_command.hpp"
#include "rendering/shader_interfaces/gl_shader.hpp"

#include <map>
#include <array>
#include <vector>

struct Mesh; // Forward Declaration

struct OpenGL_MeshData
{
public:
    void SetIndicesCount(const unsigned int IndicesCount) { _indices_count = IndicesCount; }
    void SetBaseVertex(const unsigned int BaseVertex)     { _base_vertex = BaseVertex;     }
    void SetBaseIndex(const unsigned int BaseIndex)       { _base_index = BaseIndex;       }

    unsigned int IndicesCount() const { return _indices_count; }
    unsigned int BaseVertex() const   { return _base_vertex;   }
    unsigned int BaseIndex() const    { return _base_index;    }

private:
    unsigned int _indices_count = 0;
    unsigned int _base_vertex   = 0;
    unsigned int _base_index    = 0;
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

    bool InitNewTheatre();

    void ClearBuffer(glm::vec4 ClearColor);
    const ShaderInterface* GetShader(unsigned int ShaderSelection) const;
    bool BindShader(unsigned int ShaderLabel);
    bool BuildShader(unsigned int ShaderLabel, const std::string& VertexShaderCode, const std::string& FragmentShaderCode);
    bool RebuildShader(unsigned int ShaderLabel, const std::string& VertexShaderCode, const std::string& FragmentShaderCode);
    void RenderSingleCommand(const RenderCommand& RenderCommand);

private:
    static void BufferMeshData(const int MeshID, const Mesh* NewMesh, std::vector<float>* AllVertexData, std::vector<unsigned int>* AllIndices);

    #define VAOS_AMOUNT 1
    #define VAO_DEFAULT 0
    #define VAO_DEFAULT_STRIDE 11

    typedef int MeshID;

    static std::map<MeshID, OpenGL_MeshData> gl_mesh_data;
    static std::array<unsigned int, VAOS_AMOUNT> VAOs;
    static std::map<unsigned int, GLShader> shaders;
    static unsigned int currently_bound_shader;
    static unsigned int VBO;
    static unsigned int IBO;
};

extern OpenGL_Backend singleton_OpenGL_Backend;

#endif // OPENGL_BACKEND_H
