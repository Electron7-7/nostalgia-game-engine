#include "opengl.hpp"
#include "debug.hpp"
#include "printing.hpp"
#include "../../render_command.hpp"
#include "world/world.hpp"
#include "settings/settings.hpp" // IWYU pragma: keep
#include "things/things.hpp"
#include "things/actors/light.hpp"
#include "things/resources/shaders.hpp" // IWYU pragma: keep
#include "things/resources/resource_data.hpp"
#include "things/resources/basic/mesh.hpp"
#include "things/resources/basic/texture.hpp"
#include "things/resources/complex/material.hpp" // IWYU pragma: keep
#include "things/resources/complex/mesh_instance.hpp" // IWYU pragma: keep
#include "DearImGui/imgui.h"
#include "DearImGui/imgui_impl_opengl3.h"
#include "glad/glad.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

DEBUG(int g_ShaderDebugOuptut = Shader_ALL;)

std::array<unsigned int, VAOS_AMOUNT> OpenGL_Backend::m_VAOs = {};
std::map<unsigned int, GLShader>      OpenGL_Backend::m_Shaders = {};
std::map<id_t, OpenGL_MeshData>       OpenGL_Backend::m_MeshData = {};
std::map<id_t, OpenGL_TextureID>      OpenGL_Backend::m_TextureIDs = {};

static unsigned int s_VBO = 0;
static unsigned int s_IBO = 0;

void OpenGL_Backend::ClearBuffer(glm::vec4 clear_color)
{
    glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool OpenGL_Backend::Init()
{
    if(m_IsInitialized)
        { return true; }

    PRINT_DEBUG("OpenGL_Backend::Init")
    PRINT_DEBUG("OpenGL Version: {}", (char*)glGetString(GL_VERSION))

    glGenVertexArrays(VAOS_AMOUNT, m_VAOs.data());

#   ifndef CLANGD_KEEPS_CRASHING_HERE
        BuildShader(Shaders::BlinnPhong, GLSL_BlinnPhong_Vert, GLSL_BlinnPhong_Frag);
        BuildShader(Shaders::Fullbright, GLSL_BlinnPhong_Vert, GLSL_FullBright_Frag);
#   endif

    World::SetUp(glm::vec3(0.0f, 1.0f, 0.0f));
    World::SetRight(glm::vec3(1.0f, 0.0f, 0.0f));
    World::SetFront(glm::vec3(0.0f, 0.0f, -1.0f));

    #pragma message("Implement OpenGL Debug Output")
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEPTH_TEST);

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
    if(mesh->Status() == ResourceStatus::SUCCESSFUL)
        { m_MeshData[mesh->GetID()] = OpenGL_MeshData{mesh->GetVertexData(), mesh->GetIndices()}; }
}

void OpenGL_Backend::BufferTexture(Texture* texture)
{
    #pragma message("TODO: Only flip images that need to be flipped")
    stbi_set_flip_vertically_on_load(true);

    unsigned int id = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &id);

    int l_Width, l_Height, l_Channels;
    unsigned char* l_Data = stbi_load_from_memory(texture->Data().Data(), texture->Data().Size(), &l_Width, &l_Height, &l_Channels, STBI_rgb);

    if(!l_Data)
    {
        PRINT_ERROR("OpenGL_Backend::BufferTexture - Failed to load Texture '{}' (ID#{})", texture->GetName(), texture->GetID())
        glDeleteTextures(1, &id);
        return;
    }

    #pragma message("FIXME: I don't know how to properly use 'glTextureSubImage2D' yet")
    glBindTexture(GL_TEXTURE_2D, id);
    glTextureStorage2D(id, 1, GL_RGB, l_Width, l_Height);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB, l_Width, l_Height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, l_Width, l_Height , 0, GL_RGB, GL_UNSIGNED_BYTE, l_Data);
    glGenerateTextureMipmap(id);
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameterf(id, GL_TEXTURE_MAX_ANISOTROPY, 16);
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    m_TextureIDs[texture->GetID()] = id;
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

        l_AllVertexData.insert(l_AllVertexData.end(), data.vertex_data.begin(), data.vertex_data.end());
        l_AllIndices.insert(l_AllIndices.end(), data.indices.begin(), data.indices.end());

        data.indices.clear();
        data.vertex_data.clear();
    }

    glBufferData(GL_ARRAY_BUFFER, l_AllVertexData.size() * sizeof(float), l_AllVertexData.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, l_AllIndices.size() * sizeof(unsigned int), l_AllIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VAO_DEFAULT_STRIDE * sizeof(float), (void*)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VAO_DEFAULT_STRIDE * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VAO_DEFAULT_STRIDE * sizeof(float), (void*)(6 * sizeof(float)));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, VAO_DEFAULT_STRIDE * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
}

void OpenGL_Backend::DestroyRenderingData()
{
    #pragma message("(FIXME) There's more to do here than just this")
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

bool OpenGL_Backend::BuildShader(unsigned int shader, const char* vertex_shader_code, const char* fragment_shader_code)
{
    if(m_Shaders.contains(shader))
        { glDeleteShader(m_Shaders.at(shader).ID()); }
    m_Shaders[shader] = GLShader();
    std::string vertex_shader(vertex_shader_code);
    std::string fragment_shader(fragment_shader_code);
    return m_Shaders.at(shader).CompileShader(vertex_shader, fragment_shader);
}

bool OpenGL_Backend::BindShader(unsigned int shader)
{
    if(!m_Shaders.contains(shader))
    {
        PRINT_WARNING("OpenGL_Backend::BindShader - invalid shader label! Returning false")
        return false;
    }
    glUseProgram(m_Shaders.at(shader).ID());
    s_CurrentlyBoundShader = shader;
    return true;
}

bool OpenGL_Backend::DeleteShader(unsigned int shader)
{
    if(!m_Shaders.contains(shader) || shader == Shaders::BlinnPhong)
        { return false; }
    BindShader(Shaders::BlinnPhong);
    glDeleteShader(m_Shaders.at(shader).ID());
    m_Shaders.erase(shader);
    return true;
}

#pragma message("(FIXME) Is it a good idea to let outside code access shader interfaces directly?")
const ShaderInterface* OpenGL_Backend::GetShader(unsigned int shader_selection) const
{
    if(!m_Shaders.contains(shader_selection))
    {
        PRINT_ERROR("OpenGL_Backend::GetShader - Invalid shader ID: '{}'. Returning the safety shader", shader_selection)
        shader_selection = Shaders::BlinnPhong;
    }
    return &m_Shaders.at(shader_selection);
}

void OpenGL_Backend::BufferLight(light_t* light, unsigned int shader)
{
    #pragma message("Currently, lights are limited to a maximum number but that number isn't enforced in the code and can overflow")
    std::string l_Light = std::format("_lights[{}].", light->Index());
    switch(light->Type())
    {
    case LightType::POINT:
        l_Light = "point" + l_Light;
        break;
    case LightType::SPOT:
        l_Light = "spot" + l_Light;
        break;
    case LightType::DIRECTIONAL:
        l_Light = "directional" + l_Light;
        break;
    }

    GetShader(shader)->SetUniform(l_Light + "color",             light->m_Color);
    GetShader(shader)->SetUniform(l_Light + "energy",            light->m_Energy);
    GetShader(shader)->SetUniform(l_Light + "specular_strength", light->m_SpecularStrength);
    GetShader(shader)->SetUniform(l_Light + "ambient_strength",  light->m_AmbientStrength);
    GetShader(shader)->SetUniform(l_Light + "attenuation",       light->m_Attenuation);
    GetShader(shader)->SetUniform(l_Light + "range",             light->m_Range);
    GetShader(shader)->SetUniform(l_Light + "position",          light->cOrigin());
    GetShader(shader)->SetUniform(l_Light + "direction",         light->Front());
    GetShader(shader)->SetUniform(l_Light + "spot_cutoff",       light->m_SpotAngle);
    GetShader(shader)->SetUniform(l_Light + "spot_cutoff_fade",  light->m_SpotAngleFade);
}

void OpenGL_Backend::RenderSingleCommand(const RenderCommand& rendercmd)
{
    auto mesh_instance = g_GetThing<MeshInstance>(rendercmd.m_MeshInstanceID);
    auto material      = g_GetThing<Material>(mesh_instance->GetMaterialID());

    glBindVertexArray(m_VAOs.at(VAO_DEFAULT));
    BindShader(rendercmd.m_ShaderID);

    if(!material->m_DontUseTexture)
        { glEnable(GL_FRAMEBUFFER_SRGB); }
    glEnable(GL_BLEND);

    glBindTextureUnit(0, GetTextureID(material->GetDiffuseTexture()));
    glBindTextureUnit(1, GetTextureID(material->GetSpecularTexture()));

    DEBUG(GetShader(rendercmd.m_ShaderID)->SetUniform("debug_output", g_ShaderDebugOuptut);)

    GetShader(rendercmd.m_ShaderID)->SetUniform("point_lights_count", PointLight::GetCount());
    GetShader(rendercmd.m_ShaderID)->SetUniform("spot_lights_count", SpotLight::GetCount());
    GetShader(rendercmd.m_ShaderID)->SetUniform("directional_lights_count", DirectionalLight::GetCount());

    GetShader(rendercmd.m_ShaderID)->SetUniform("model_matrix", rendercmd.m_ModelMatrix);
    GetShader(rendercmd.m_ShaderID)->SetUniform("normal_matrix", glm::mat3(glm::transpose(glm::inverse(rendercmd.m_ModelMatrix))));
    GetShader(rendercmd.m_ShaderID)->SetUniform("projection_matrix", rendercmd.ProjectionMatrix());
    GetShader(rendercmd.m_ShaderID)->SetUniform("view_matrix", rendercmd.ViewMatrix());
    GetShader(rendercmd.m_ShaderID)->SetUniform("view_position", rendercmd.ViewPosition());

    GetShader(rendercmd.m_ShaderID)->SetUniform("current_material.texture_diffuse",  0);
    GetShader(rendercmd.m_ShaderID)->SetUniform("current_material.texture_specular", 1);
    GetShader(rendercmd.m_ShaderID)->SetUniform("current_material.use_textures", !material->m_DontUseTexture);
    GetShader(rendercmd.m_ShaderID)->SetUniform("current_material.diffuse_color", material->m_Color);
    GetShader(rendercmd.m_ShaderID)->SetUniform("current_material.alpha", material->m_Alpha);
    GetShader(rendercmd.m_ShaderID)->SetUniform("current_material.specular_sharpness", material->m_SpecularSharpness);
    GetShader(rendercmd.m_ShaderID)->SetUniform("current_material.specular_strength", material->m_SpecularStrength);

    OpenGL_MeshData* data = GetMeshData(mesh_instance->GetMeshID());

    glDrawElementsBaseVertex(GL_TRIANGLES, data->indices_count, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * data->base_index), data->base_vertex);

    glDisable(GL_FRAMEBUFFER_SRGB);
}

unsigned int OpenGL_Backend::GetTextureID(id_t id)
{
    if(!m_TextureIDs.contains(id))
        { return m_TextureIDs.at(Images::ID::Missing); }
    return m_TextureIDs.at(id);
}

OpenGL_MeshData* OpenGL_Backend::GetMeshData(id_t id)
{
    if(!m_MeshData.contains(id))
        { return &m_MeshData.at(Models::ID::Error); }
    return &m_MeshData.at(id);
}
