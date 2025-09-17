#include "opengl.hpp"
#include "printing.hpp"
#include "world/world.hpp"
#include "filesystem/file_data.hpp" // IWYU pragma: keep
#include "rendering/shader_interfaces/shader_interface.hpp"
#include "rendering/shader_interfaces/gl_shader.hpp"
#include "things/resources/resource_data.hpp"
#include "things/resources/basic/mesh.hpp"
#include "things/resources/basic/texture.hpp"
#include "things/resources/complex/material.hpp" // IWYU pragma: keep
#include "DearImGui/imgui.h"
#include "DearImGui/imgui_impl_opengl3.h"
#include "glad/glad.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

std::array<unsigned int, VAOS_AMOUNT> OpenGL_Backend::m_VAOs = {};
std::map<unsigned int, GLShader>      OpenGL_Backend::m_Shaders = {};
std::map<id_t, OpenGL_MeshData>       OpenGL_Backend::m_MeshData = {};
std::map<id_t, OpenGL_TextureID>      OpenGL_Backend::m_TextureIDs = {};

unsigned int OpenGL_Backend::m_CurrentlyBoundShader = Shaders::Safety;

static unsigned int s_VBO = 0;
static unsigned int s_IBO = 0;

static std::set<id_t> s_InUseIDs = {};

void OpenGL_Backend::ClearBuffer(glm::vec4 clear_color)
{
    glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool OpenGL_Backend::Init()
{
    if(m_IsInitialized)
        { return true; }

    glGenVertexArrays(VAOS_AMOUNT, m_VAOs.data());

    if(!BuildShader(Shaders::Safety, GLSL::SafetyShader_Vert, GLSL::SafetyShader_Frag))
        { return false; } // The safety shader must ALWAYS compile
    BuildShader(Shaders::BlinnPhong, GLSL::BlinnPhong_Vert, GLSL::BlinnPhong_Frag);

    World::SetUp(glm::vec3(0.0f, 1.0f, 0.0f));
    World::SetRight(glm::vec3(1.0f, 0.0f, 0.0f));
    World::SetFront(glm::vec3(0.0f, 0.0f, -1.0f));

    m_IsInitialized = true;
    return true;
}

bool OpenGL_Backend::InitImGui()
{
    if(is_imgui_initialized)
        { return true; }

    if(!ImGui_ImplOpenGL3_Init())
    {
        PRINT_ERROR("GLFW_Backend::InitImGui - ImGui_ImplOpenGL3_Init returned false!")
        return false;
    }

    is_imgui_initialized = true;
    return true;
}

void OpenGL_Backend::ImGuiNewFrame()
{ ImGui_ImplOpenGL3_NewFrame(); }

void OpenGL_Backend::ImGuiRender()
{ ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); }

void OpenGL_Backend::BufferMesh(Mesh* mesh)
{
    m_MeshData[mesh->GetID()] = OpenGL_MeshData
    {
        mesh->GetVertexData(),
        mesh->GetIndices(),
        (unsigned int)mesh->GetIndices().size(),
        0,
        0
    };
}

void OpenGL_Backend::BufferTexture(Texture* texture)
{
    unsigned int id = 0;
    stbi_set_flip_vertically_on_load(texture->Data()->HasPath()); // FIXME: Idk if this is accurate

    // glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureIDs.at(texture->GetID()));
    // glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTextureParameteri(id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // glTextureParameterf(id, GL_TEXTURE_MAX_ANISOTROPY, 16);
    // glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    // glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int l_Width, l_Height, l_Channels;
    unsigned char* l_Data = stbi_load_from_memory(texture->Data()->data(), texture->Data()->size(), &l_Width, &l_Height, &l_Channels, STBI_rgb);

#   ifdef DEBUGGING // Debugging Textures
        if(l_Data)
        {
            switch(texture->Data()->Type())
            {
            case FileType::image_JPG:
                stbi_write_jpg(("STB_" + texture->GetName() + ".jpg").c_str(), l_Width, l_Height, l_Channels, l_Data, 90);
                break;
            case FileType::image_PNG:
                stbi_write_png(("STB_" + texture->GetName() + ".png").c_str(), l_Width, l_Height, l_Channels, l_Data, l_Width * l_Channels);
                break;
            default:
                break;
            }
        }
#   endif // DEBUGGING

    if(!l_Data)
    {
        PRINT_ERROR("OpenGL_Backend::BufferTexture - Failed to load Texture '{}'!", texture->GetName())
        // glDeleteTextures(1, &m_TextureIDs.at(texture->GetID()));
        m_TextureIDs.erase(texture->GetID());
        return;
    }

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB, l_Width, l_Height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, l_Width, l_Height , 0, GL_RGB, GL_UNSIGNED_BYTE, l_Data);
    glGenerateMipmap(GL_TEXTURE_2D);
    m_TextureIDs[texture->GetID()] = id;
    // glTextureStorage2D(id, 1, GL_RGB, l_Width, l_Height);
    // glTextureSubImage2D(id, 0, 0, 0, l_Width, l_Height, GL_RGB, GL_UNSIGNED_BYTE, l_Data);
    // glGenerateTextureMipmap(id);
    stbi_image_free(l_Data);
}

void OpenGL_Backend::CreateRenderingData()
{
    std::vector<float> l_AllVertexData = {};
    std::vector<unsigned int> l_AllIndices = {};

    glBindVertexArray(m_VAOs.at(VAO_DEFAULT));

    glDeleteBuffers(1, &s_VBO);
    glDeleteBuffers(1, &s_IBO);
    glGenBuffers(1, &s_VBO);
    glGenBuffers(1, &s_IBO);
    glBindBuffer(GL_ARRAY_BUFFER, s_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_IBO);

    for(auto& [id, data] : m_MeshData)
    {
        data.base_vertex   = (l_AllVertexData.size() / VAO_DEFAULT_STRIDE);
        data.base_index    = l_AllIndices.size();
        data.indices_count = data.indices.size();

        l_AllVertexData.insert(l_AllVertexData.end(), data.vertex_data.begin(), data.vertex_data.end());
        l_AllIndices.insert(l_AllIndices.end(), data.indices.begin(), data.indices.end());

        data.indices.clear();
        data.vertex_data.clear();
    }

    glBufferData(GL_ARRAY_BUFFER, l_AllVertexData.size() * sizeof(float), l_AllVertexData.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, l_AllIndices.size() * sizeof(unsigned int), l_AllIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
}

void OpenGL_Backend::DestroyRenderingData()
{
    // FIXME: There's more to do here than just this
    m_MeshData.clear();
}

void OpenGL_Backend::Shutdown()
{
    assert(m_IsInitialized);

    if(is_imgui_initialized)
    { ImGui_ImplOpenGL3_Shutdown(); }

    m_IsInitialized = false;
    is_imgui_initialized = false;
}

bool OpenGL_Backend::BindShader(unsigned int shader_label)
{
    if(!m_Shaders.contains(shader_label))
    {
        PRINT_WARNING("OpenGL_Backend::BindShader - invalid shader label! Returning false")
        return false;
    }

    if(m_CurrentlyBoundShader == shader_label)
        { return true; }

    // FIXME: Can this fail even if the shaders are compiled successfuly?
    m_Shaders.at(m_CurrentlyBoundShader).Unbind();
    m_Shaders.at(shader_label).Bind();
    m_CurrentlyBoundShader = shader_label;
    return true;
}

bool OpenGL_Backend::BuildShader(unsigned int shader_label, const char* vertex_shader_code, const char* fragment_shader_code)
{
    if(m_Shaders.contains(shader_label))
    {
        if(shader_label == m_CurrentlyBoundShader)
        {
            m_Shaders.at(shader_label).Unbind();
            m_Shaders.at(Shaders::Safety).Bind();
        }
        m_Shaders.at(shader_label).Delete();
    }

    m_Shaders[shader_label] = GLShader();
    std::string vertex_shader(vertex_shader_code);
    std::string fragment_shader(fragment_shader_code);
    return m_Shaders.at(shader_label).CompileShader(vertex_shader, fragment_shader);
}

unsigned int OpenGL_Backend::GetTextureID(id_t id)
{
    if(!m_TextureIDs.contains(id))
        { return m_TextureIDs.at(0); }
    return m_TextureIDs.at(id);
}

// FIXME: Is it a good idea to let outside code access shader interfaces directly?
const ShaderInterface* OpenGL_Backend::GetShader(unsigned int shader_selection) const
{
    if(!m_Shaders.contains(shader_selection))
    {
        PRINT_ERROR("OpenGL_Backend::GetShader - Invalid shader ID: '{}'. Returning the safety shader", shader_selection)
        shader_selection = Shaders::Safety;
    }

    return &m_Shaders.at(shader_selection);
}

void OpenGL_Backend::RenderSingleCommand(const RenderCommand& rendercmd)
{
    const RenderContext& context = rendercmd.GetRenderContext();
    auto material = rendercmd.GetMaterial();

    glBindVertexArray(m_VAOs.at(VAO_DEFAULT));
    BindShader(context.GetShaderID());

    glEnable(GL_BLEND);

    glBindTextureUnit(0, GetTextureID(material->GetDiffuseTexture()));
    glBindTextureUnit(1, GetTextureID(material->GetSpecularTexture()));

    GetShader(context.GetShaderID())->SetUniform("point_lights_count", context.GetPointLightsCount());
    GetShader(context.GetShaderID())->SetUniform("spot_lights_count", context.GetSpotLightsCount());
    GetShader(context.GetShaderID())->SetUniform("directional_lights_count", context.GetDirectionalLightsCount());

    GetShader(context.GetShaderID())->SetUniform("model_matrix", context.GetModelMatrix());
    GetShader(context.GetShaderID())->SetUniform("view_matrix", rendercmd.ViewMatrix());
    GetShader(context.GetShaderID())->SetUniform("projection_matrix", rendercmd.ProjectionMatrix());
    GetShader(context.GetShaderID())->SetUniform("normal_matrix", glm::mat3(glm::transpose(glm::inverse(context.GetModelMatrix()))));
    GetShader(context.GetShaderID())->SetUniform("view_position", rendercmd.ViewPosition());

    GetShader(context.GetShaderID())->SetUniform("current_material.texture_diffuse",  0);
    GetShader(context.GetShaderID())->SetUniform("current_material.texture_specular", 1);
    GetShader(context.GetShaderID())->SetUniform("current_material.diffuse_color", material->m_Color);
    GetShader(context.GetShaderID())->SetUniform("current_material.alpha", material->m_Alpha);
    GetShader(context.GetShaderID())->SetUniform("current_material.specular_sharpness", material->m_SpecularSharpness);
    GetShader(context.GetShaderID())->SetUniform("current_material.specular_strength", material->m_SpecularStrength);

    // FIXME: Make this safer
    const OpenGL_MeshData& data = m_MeshData.at(rendercmd.GetMeshID());

    glDrawElementsBaseVertex(GL_TRIANGLES, data.indices_count, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * data.base_index), data.base_vertex);
}
