#include "opengl.hpp"
#include "printing.hpp"
#include "theatre/3d_common.hpp"
#include "rendering/shader_interfaces/shader_interface.hpp"
#include "rendering/shader_interfaces/gl_shader.hpp"
#include "embedded/opengl_shaders.hpp"
#include "DearImGui/imgui.h"
#include "DearImGui/imgui_impl_opengl3.h"
#include "glad/glad.h"

std::array<unsigned int, VAOS_AMOUNT> OpenGL_Backend::VAOs = {};
std::map<unsigned int, GLShader> OpenGL_Backend::shaders = {};
unsigned int OpenGL_Backend::currently_bound_shader = Shaders::SAFETY;
unsigned int OpenGL_Backend::VBO = 0;
unsigned int OpenGL_Backend::IBO = 0;

void OpenGL_Backend::ClearBuffer(glm::vec4 clear_color)
{
    glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


bool OpenGL_Backend::Init()
{
    if(is_initialized)
    { return true; }

    glGenVertexArrays(VAOS_AMOUNT, VAOs.data());

    // The safety shader must ALWAYS compile; the rest can fail without causing crashes
    if(!BuildShader(Shaders::SAFETY, glsl_VERT_SafetyShader, glsl_FRAG_SafetyShader)) return false;
    BuildShader(Shaders::BLINN_PHONG, glsl_VERT_BlinnPhong, glsl_FRAG_BlinnPhong);

    // TODO: See note in 'src/world/3d_common.hpp'
    World::Orientation::SetWorldUp(glm::vec3(0.0f, 1.0f, 0.0f));
    World::Orientation::SetWorldRight(glm::vec3(1.0f, 0.0f, 0.0f));
    World::Orientation::SetWorldFront(glm::vec3(0.0f, 0.0f, -1.0f));

    is_initialized = true;
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

bool OpenGL_Backend::InitNewTheatre()
{
    if(is_theatre_data_initialized)
    { return true; }

    // FIXME: Make this shit safer

    std::vector<float> all_vertex_data = {};
    std::vector<unsigned int> all_indices = {};

    glBindVertexArray(VAOs.at(VAO_DEFAULT));
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // std::map<MeshWrapper::MeshID, const Mesh*> all_meshes = g_pTheatreManager->GetAllCurrentMeshes();

    // for(auto& [mesh_id, mesh_ptr] : all_meshes)
        // BufferMeshData(mesh_id, mesh_ptr, &all_vertex_data, &all_indices);

    glBufferData(GL_ARRAY_BUFFER, all_vertex_data.size() * sizeof(float), all_vertex_data.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, all_indices.size() * sizeof(unsigned int), all_indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    is_theatre_data_initialized = true;
    return true;
}

void OpenGL_Backend::Shutdown()
{ is_initialized = false; }

bool OpenGL_Backend::BindShader(unsigned int shader_label)
{
    if(!shaders.contains(shader_label))
    {
        PRINT_WARNING("OpenGL_Backend::BindShader - invalid shader label! Returning false")
        return false;
    }

    if(currently_bound_shader == shader_label)
    { return true; }

    // FIXME: Can this fail even if the shaders are compiled successfuly?
    shaders.at(currently_bound_shader).Unbind();
    shaders.at(shader_label).Bind();
    currently_bound_shader = shader_label;
    return true;
}

void OpenGL_Backend::ImGuiNewFrame()
{ ImGui_ImplOpenGL3_NewFrame(); }

void OpenGL_Backend::ImGuiRender()
{ ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); }

bool OpenGL_Backend::BuildShader(unsigned int shader_label, const std::string& vertex_shader_code, const std::string& fragment_shader_code)
{
    if(shaders.contains(shader_label))
    {
        PRINT_WARNING("OpenGL_Backend::BuildShader - a shader with the supplied label already exists; returning result of ::IsValid")
        return shaders.at(shader_label).IsValid();
    }

    shaders[shader_label] = GLShader();
    return shaders.at(shader_label).CompileShader(vertex_shader_code, fragment_shader_code);
}

bool OpenGL_Backend::RebuildShader(unsigned int shader_label, const std::string& vertex_shader_code, const std::string& fragment_shader_code)
{
    if(!shaders.contains(shader_label))
    {
        PRINT_WARNING("OpenGL_Backend::RebuildShader - invalid shader label! Returning false")
        return false;
    }

    if(shader_label == currently_bound_shader)
    {
        shaders.at(shader_label).Unbind();
        shaders.at(Shaders::SAFETY).Bind();
    }

    shaders.at(shader_label).Delete();
    bool was_successful = shaders.at(shader_label).CompileShader(vertex_shader_code, fragment_shader_code);

    if(shader_label == currently_bound_shader && was_successful)
    {
        shaders.at(Shaders::SAFETY).Unbind();
        shaders.at(shader_label).Bind();
    }

    return was_successful;
}

// FIXME: Is it a good idea to let outside code access shader interfaces directly?
const ShaderInterface* OpenGL_Backend::GetShader(unsigned int shader_selection) const
{
    if(!shaders.contains(shader_selection))
    {
        PRINT_ERROR("OpenGL_Backend::GetShader - Invalid shader ID: '{}'. Returning the safety shader", shader_selection)
        shader_selection = Shaders::SAFETY;
    }

    return &shaders.at(shader_selection);
}

void OpenGL_Backend::RenderSingleCommand(const RenderCommand& rendercmd)
{
    RenderContext context = rendercmd.GetRenderContext();

    glBindVertexArray(VAOs.at(VAO_DEFAULT));

    GetShader(context.GetShaderID())->SetUniform("point_lights_count", context.GetPointLightsCount());
    GetShader(context.GetShaderID())->SetUniform("spot_lights_count", context.GetSpotLightsCount());
    GetShader(context.GetShaderID())->SetUniform("directional_lights_count", context.GetDirectionalLightsCount());

    glEnable(GL_BLEND);

    BindShader(context.GetShaderID());

    GetShader(context.GetShaderID())->SetUniform("model_matrix", context.GetModelMatrix());
    GetShader(context.GetShaderID())->SetUniform("normal_matrix", glm::mat3(glm::transpose(glm::inverse(context.GetModelMatrix()))));
    GetShader(context.GetShaderID())->SetUniform("view_matrix", context.GetViewMatrix());
    GetShader(context.GetShaderID())->SetUniform("projection_matrix", context.GetProjectionMatrix());
    GetShader(context.GetShaderID())->SetUniform("view_position", context.GetViewPosition());

    // const OpenGL_MeshData* mesh_data = &gl_mesh_data.at(context.GetMeshID());

    // glDrawElementsBaseVertex(GL_TRIANGLES, mesh_data->IndicesCount(), GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh_data->BaseIndex()), mesh_data->BaseVertex());
}

void OpenGL_Backend::BufferMeshData(const int mesh_id, const Mesh* mesh, std::vector<float>* all_vertex_data, std::vector<unsigned int>* all_indices)
{
    // FIXME: Make this shit safer

    // gl_mesh_data[mesh_id] = OpenGL_MeshData();
    // gl_mesh_data.at(mesh_id).SetBaseVertex(all_vertex_data->size() / VAO_DEFAULT_STRIDE);
    // gl_mesh_data.at(mesh_id).SetBaseIndex(all_indices->size());

    // std::vector<float> vertex_data = mesh->GetVertexData();
    // std::vector<unsigned int> indices = mesh->GetIndices();

    // all_vertex_data->insert(all_vertex_data->end(), vertex_data.begin(), vertex_data.end());
    // all_indices->insert(all_indices->end(), indices.begin(), indices.end());
}
