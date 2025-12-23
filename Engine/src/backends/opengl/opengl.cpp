#define DO_NOT_COMPILE
#ifndef DO_NOT_COMPILE
#include "opengl.hpp"
#include "embedded/shaders.hpp" // IWYU pragma: keep // see the `clangd` target in the Makefile
#include "managers/theatre_manager.hpp"
#include "renderer/render_command.hpp"
#include "filesystem/file_data.hpp"
#include "things/actors/light.hpp"
#include "things/actors/nostalgia_player.hpp" // IWYU pragma: keep
#include "things/devices/material.hpp"
#include "things/devices/mesh_instance.hpp"
#include "settings/graphics.hpp"
#include "settings/player.hpp"
#include "common/printing.hpp"
#include "thirdparty/DearImGui/imgui_impl_opengl3.h"
#include "thirdparty/glad/glad.h"
#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb_image/stb_image.h"

#include <glm/ext/matrix_clip_space.hpp>

#ifdef DEBUGGING
    static void APIENTRY OpenGL_DebugMessageCallback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const void*);
#endif

int g_ShaderDebugOuptut = Shader_ALL;
static std::vector<OpenGL_BufferData> sBufferData{};
static uint sVBO{0};
static uint sIBO{0};

bool OpenGL_Backend::Init()
{
    PRINT_PRETTY_FUNCTION;
    print_debug("OpenGL Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

    glGenVertexArrays(VAOS_AMOUNT, mVAOs.data());

#ifndef CLANGD_KEEPS_CRASHING_HERE
        BuildShader(Shaders::BlinnPhong, GLSL_BlinnPhong_Vert, GLSL_BlinnPhong_Frag);
        BuildShader(Shaders::Fullbright, GLSL_BlinnPhong_Vert, GLSL_FullBright_Frag);
#endif

    // Settings::World::UpdateOrientation();
    SetWindowViewport(Viewport{Settings::Window::FramebufferSize(), Settings::Window::FramebufferPosition()});

#ifdef DEBUGGING
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(OpenGL_DebugMessageCallback, nullptr);
#endif
    glEnable(GL_DEPTH_TEST);
    return true;
}

bool OpenGL_Backend::InitImGui()
{
    if(!ImGui_ImplOpenGL3_Init())
        { return print_error("ImGui_ImplOpenGL3_Init returned false!"); }
    return true;
}

void OpenGL_Backend::ImGuiNewFrame()
{ ImGui_ImplOpenGL3_NewFrame(); }

void OpenGL_Backend::ImGuiRender()
{ glDisable(GL_FRAMEBUFFER_SRGB); ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); }

void OpenGL_Backend::BufferMesh(const FileData& data, const ID& id)
{
    OpenGL_BufferData temp_data{};
    if(GraphicsBackend::CreateMeshData(temp_data.vertices, temp_data.indices, data))
    {
        temp_data.id = id;
        sBufferData.push_back(temp_data);
    }
}

void OpenGL_Backend::BufferTexture(const FileData& data, const ID& texture_id)
{
#pragma message("TODO: Only flip images that need to be flipped")
    stbi_set_flip_vertically_on_load(true);

    uint id{0};
    glCreateTextures(GL_TEXTURE_2D, 1, &id);

    int l_Width, l_Height, l_Channels;
    unsigned char* l_Data = stbi_load_from_memory(data.Data(), data.Size(), &l_Width, &l_Height, &l_Channels, STBI_rgb);

    if(!l_Data)
    {
        print_error("Failed to load Texture");
        glDeleteTextures(1, &id);
        return;
    }

    // glTextureStorage2D(id, 1, GL_RGB, l_Width, l_Height);
#pragma message("FIXME: I don't know how to properly use 'glTextureSubImage2D' yet")
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, l_Width, l_Height , 0, GL_RGB, GL_UNSIGNED_BYTE, l_Data);
    glGenerateTextureMipmap(id);
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameterf(id, GL_TEXTURE_MAX_ANISOTROPY, 16);
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    mTextureIDs[texture_id] = id;
    stbi_image_free(l_Data);
}

void OpenGL_Backend::CreateRenderingData()
{
    glBindVertexArray(mVAOs.at(VAO_DEFAULT));
    glDeleteBuffers(1, &sVBO);
    glDeleteBuffers(1, &sIBO);
    glGenBuffers(1, &sVBO);
    glGenBuffers(1, &sIBO);
    glBindBuffer(GL_ARRAY_BUFFER, sVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sIBO);

    std::vector<float> l_AllVertexData = {};
    std::vector<uint> l_AllIndices = {};

    for(const auto& data : sBufferData)
    {
        mMeshData[data.id] =
        {
            static_cast<uint>(data.indices.size()),
            static_cast<uint>(l_AllVertexData.size() / VAO_DEFAULT_STRIDE),
            static_cast<uint>(l_AllIndices.size())
        };
        l_AllVertexData.insert(l_AllVertexData.end(), data.vertices.begin(), data.vertices.end());
        l_AllIndices.insert(l_AllIndices.end(), data.indices.begin(), data.indices.end());
    }

    sBufferData.clear();

    glBufferData(GL_ARRAY_BUFFER,
        l_AllVertexData.size() * sizeof(float),
        l_AllVertexData.data(),
        GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        l_AllIndices.size() * sizeof(uint),
        l_AllIndices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0,
        3,
        GL_FLOAT,
        GL_FALSE,
        VAO_DEFAULT_STRIDE * sizeof(float),
        (void*)(0));
    glVertexAttribPointer(1,
        3,
        GL_FLOAT,
        GL_FALSE,
        VAO_DEFAULT_STRIDE * sizeof(float),
        (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2,
        3,
        GL_FLOAT,
        GL_FALSE,
        VAO_DEFAULT_STRIDE * sizeof(float),
        (void*)(6 * sizeof(float)));
    glVertexAttribPointer(3,
        2,
        GL_FLOAT,
        GL_FALSE,
        VAO_DEFAULT_STRIDE * sizeof(float),
        (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
}

void OpenGL_Backend::DestroyRenderingData()
{
    mTextureIDs.clear();
    mMeshData.clear();
}

void OpenGL_Backend::Shutdown()
{}

void OpenGL_Backend::ShutdownImGui()
{ ImGui_ImplOpenGL3_Shutdown(); }

bool OpenGL_Backend::BuildShader(const ID& shader, const char* vertex_shader_code, const char* fragment_shader_code)
{
    if(mShaders.contains(shader))
        { glDeleteShader(static_cast<uint>(mShaders.at(shader).id())); }
    mShaders[shader] = GLShader();
    std::string vertex_shader(vertex_shader_code);
    std::string fragment_shader(fragment_shader_code);
    return mShaders.at(shader).CompileShader(vertex_shader, fragment_shader);
}

bool OpenGL_Backend::BindShader(const ID& shader)
{
    if(!mShaders.contains(shader))
    {
        print_warning("invalid shader label! Returning false");
        return false;
    }
    glUseProgram(static_cast<uint>(mShaders.at(shader).id()));
    mCurrentlyBoundShader = shader;
    return true;
}

bool OpenGL_Backend::DeleteShader(const ID& shader)
{
    if(!mShaders.contains(shader) || shader == Shaders::BlinnPhong)
        { return false; }
    BindShader(Shaders::BlinnPhong);
    glDeleteShader(static_cast<uint>(mShaders.at(shader).id()));
    mShaders.erase(shader);
    return true;
}

const ShaderInterface* OpenGL_Backend::GetShader(const ID& shader_selection) const
{
    ID selected_shader{shader_selection};
    if(!mShaders.contains(shader_selection))
    {
        print_error("Invalid shader ID: '{}'. Returning the safety shader", shader_selection);
        selected_shader = Shaders::BlinnPhong;
    }
    return &mShaders.at(shader_selection);
}

void OpenGL_Backend::SetLight(light_t* light, const ID& shader)
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
    GetShader(shader)->SetUniform(l_Light + "color",             light->mColor);
    GetShader(shader)->SetUniform(l_Light + "energy",            light->mEnergy);
    GetShader(shader)->SetUniform(l_Light + "specular_strength", light->mSpecularStrength);
    GetShader(shader)->SetUniform(l_Light + "ambient_strength",  light->mAmbientStrength);
    GetShader(shader)->SetUniform(l_Light + "attenuation",       light->mAttenuation);
    GetShader(shader)->SetUniform(l_Light + "range",             light->mRange);
    GetShader(shader)->SetUniform(l_Light + "position",          light->Origin());
    GetShader(shader)->SetUniform(l_Light + "direction",         light->Front());
    GetShader(shader)->SetUniform(l_Light + "spot_cutoff",       glm::cos(glm::radians(light->mSpotAngle)));
    GetShader(shader)->SetUniform(l_Light + "spot_cutoff_fade",  glm::cos(glm::radians(light->mSpotAngle - light->mSpotAngleFade)));
}

void OpenGL_Backend::RenderSingleCommand(const RenderCommand& rendercmd)
{
    auto mesh_instance{g_pTheatreManager->GetThing<MeshInstance>(rendercmd.mesh_instance)};
    auto material{g_pTheatreManager->GetThing<Material>(mesh_instance->GetMaterialID())};
    auto player{g_pTheatreManager->GetLocalPlayer()};
    ID shader{rendercmd.shader};

    glm::mat4 projection_matrix{glm::perspective(
        glm::radians(Settings::Player::FOV),
        CurrentViewport().scale.AspectRatio(),
        Settings::Player::ViewCutoffNear,
        Settings::Player::ViewCutoffFar
    )};

    glBindVertexArray(mVAOs.at(VAO_DEFAULT));
    if(material->mFullBright)
        { shader = Shaders::Fullbright; }
    if(!material->mDontUseTexture)
        { glEnable(GL_FRAMEBUFFER_SRGB); }
    if(Settings::Graphics::GlobalWireframe || rendercmd.is_wireframe)
        { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
    glEnable(GL_BLEND);

    glBindTextureUnit(0, GetTextureID(material->GetDiffuseTexture()));
    glBindTextureUnit(1, GetTextureID(material->GetSpecularTexture()));

    GetShader(shader)->SetUniform("debug_output", g_ShaderDebugOuptut);
    GetShader(shader)->SetUniform("debug_highlight", rendercmd.debug_highlight * rendercmd.debug_highlight.a);

    GetShader(shader)->SetUniform("point_lights_count", PointLight::GetCount());
    GetShader(shader)->SetUniform("spot_lights_count", SpotLight::GetCount());
    GetShader(shader)->SetUniform("directional_lights_count", DirectionalLight::GetCount());

    GetShader(shader)->SetUniform("model_matrix", rendercmd.model_matrix);
    GetShader(shader)->SetUniform("normal_matrix", glm::mat3(glm::transpose(glm::inverse(rendercmd.model_matrix))));
    GetShader(shader)->SetUniform("projection_matrix", projection_matrix);
    GetShader(shader)->SetUniform("view_matrix", player->ViewMatrix());
    GetShader(shader)->SetUniform("view_position", player->ViewPosition());

    GetShader(shader)->SetUniform("current_material.texture_diffuse",  0);
    GetShader(shader)->SetUniform("current_material.texture_specular", 1);
    GetShader(shader)->SetUniform("current_material.use_textures", !material->mDontUseTexture);
    GetShader(shader)->SetUniform("current_material.diffuse_color", material->mColor);
    GetShader(shader)->SetUniform("current_material.alpha", material->mAlpha);
    GetShader(shader)->SetUniform("current_material.specular_sharpness", material->mSpecularSharpness);
    GetShader(shader)->SetUniform("current_material.specular_strength", material->mSpecularStrength);

    UpdateViewport(CurrentViewport());
    if(!BindShader(shader))
        { print_error("failed to bind shader '{}' ({})", shader.name(), (uint)shader); return; }

    OpenGL_MeshData* data = GetMeshData(mesh_instance->GetMeshID());
    glDrawElementsBaseVertex(GL_TRIANGLES, data->indices_count, GL_UNSIGNED_INT, (void*)(sizeof(uint) * data->base_index), data->base_vertex);

    glDisable(GL_FRAMEBUFFER_SRGB);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

uint OpenGL_Backend::GetTextureID(const ID& id)
{
    if(!mTextureIDs.contains(id))
        { return mTextureIDs.at(UniqueIDs::Reserved::i_Missing); }
    return mTextureIDs.at(id);
}

OpenGL_MeshData* OpenGL_Backend::GetMeshData(const ID& id)
{
    if(!mMeshData.contains(id))
        { return &mMeshData.at(UniqueIDs::Reserved::m_Error); }
    return &mMeshData.at(id);
}

void OpenGL_Backend::UpdateViewport(const Viewport& viewport) const
{ glViewport(viewport.position.x, viewport.position.y, viewport.scale.width, viewport.scale.height); }

//----------------------------------------------------------------------------------
// OpenGL Debug Message Callback Function
// https://gist.github.com/liam-middlebrook/c52b069e4be2d87a6d2f (with minor tweaks)
//----------------------------------------------------------------------------------
#ifdef DEBUGGING
static void APIENTRY OpenGL_DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* data)
{
    std::string _source;
    std::string _type;
    std::string _severity;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;
    default:
        _source = "UNKNOWN";
        break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;
    case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;
    default:
        _type = "UNKNOWN";
        break;
    }
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        if(!g_EnableDebugMsgHigh)
            { return; }
        _severity = "HIGH";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        if(!g_EnableDebugMsgMedium)
            { return; }
        _severity = "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        if(!g_EnableDebugMsgLow)
            { return; }
        _severity = "LOW";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        if(!g_EnableDebugMsgNotif)
            { return; }
        _severity = "NOTIFICATION";
        break;
    default:
        _severity = "UNKNOWN";
        break;
    }
    print_debug("{}: {} of {} severity, raised from {}: {}", id, _type, _severity, _source, message);;
}
#endif // DEBUGGING
#endif // DO_NOT_COMPILE
