#include "gl_renderer_api.hpp"
#include "gl_shader.hpp"        // IWYU pragma: keep // clangd crashes when processing the embedded shaders so I hide them from it
#include <Nostalgia/embedded/shaders.hpp> // IWYU pragma: keep // clangd crashes when processing the embedded shaders so I hide them from it
#include <Nostalgia/theatre/things/resources/texture.hpp>
#include <Nostalgia/theatre/things/resources/font.hpp>
#include <Nostalgia/theatre/things/thinkers/3d/light_3d.hpp>
#include <Nostalgia/application/application.hpp>
#include <Nostalgia/rendering/texture_buffer.hpp> // IWYU pragma: keep // idk why clangd says these are unused
#include <Nostalgia/rendering/vertex_array.hpp>   // IWYU pragma: keep // idk why clangd says these are unused
#include <Nostalgia/rendering/frame_buffer.hpp>   // IWYU pragma: keep // idk why clangd says these are unused
#include <Nostalgia/settings/world.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

bool gPrintDrawLogs{false};
bool gOpenGLEnableNotificationMesssages{true};
DebugMessageSeverityFilter gOpenGLMessageFilter{DebugMessageSeverityFilter::High};

#ifdef NOSTALGIA_DEBUGGING
static void APIENTRY OpenGL_DebugMessageCallback(GLenum,GLenum,GLuint,GLenum,GLsizei,const GLchar*,const void*);
#endif // NOSTALGIA_DEBUGGING

bool OpenGLRendererAPI::Init()
{
    PRINT_PRETTY_FUNCTION;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

#ifdef NOSTALGIA_DEBUGGING
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(OpenGL_DebugMessageCallback, nullptr);
#endif

    mShaders[Shaders::BlinnPhong] = Shader::Create();
    mShaders[Shaders::Fullbright] = Shader::Create();
    mShaders[Shaders::SkyBox]     = Shader::Create();
    mShaders[Shaders::Fast2D]     = Shader::Create();
    mShaders[Shaders::Fonts]      = Shader::Create();

    SetViewport(0, 0, MainWindow()->GetScale().w(), MainWindow()->GetScale().h());

#ifndef CLANGD_KEEPS_CRASHING_HERE
    mShaders[Shaders::BlinnPhong]->CompileShader(GLSL_BlinnPhong_Vert, GLSL_BlinnPhong_Frag);
    mShaders[Shaders::Fullbright]->CompileShader(GLSL_BlinnPhong_Vert, GLSL_FullBright_Frag);
    mShaders[Shaders::SkyBox]->CompileShader(GLSL_SkyBox_Vert, GLSL_SkyBox_Frag);
    mShaders[Shaders::Fast2D]->CompileShader(GLSL_Fast2D_Vert, GLSL_Fast2D_Frag);
    mShaders[Shaders::Fonts]->CompileShader(GLSL_Font_Vert, GLSL_Font_Frag);
#endif

    EnumPrettifier::Assign(TEXTURE_TYPE_1D,         "TEXTURE_TYPE_1D");
    EnumPrettifier::Assign(TEXTURE_TYPE_2D,         "TEXTURE_TYPE_2D");
    EnumPrettifier::Assign(TEXTURE_TYPE_3D,         "TEXTURE_TYPE_3D");
    EnumPrettifier::Assign(TEXTURE_TYPE_1D_ARRAY,   "TEXTURE_TYPE_1D_ARRAY");
    EnumPrettifier::Assign(TEXTURE_TYPE_2D_ARRAY,   "TEXTURE_TYPE_2D_ARRAY");
    EnumPrettifier::Assign(TEXTURE_TYPE_3D_ARRAY,   "TEXTURE_TYPE_3D_ARRAY");
    EnumPrettifier::Assign(TEXTURE_TYPE_CUBE,       "TEXTURE_TYPE_CUBE");
    EnumPrettifier::Assign(TEXTURE_TYPE_CUBE_ARRAY, "TEXTURE_TYPE_CUBE_ARRAY");
    EnumPrettifier::Assign(TEXTURE_TYPE_NONE,       "TEXTURE_TYPE_NONE");

    EnumPrettifier::Assign(SAMPLER_FILTER_NEAREST, "SAMPLER_FILTER_NEAREST");
    EnumPrettifier::Assign(SAMPLER_FILTER_LINEAR,  "SAMPLER_FILTER_LINEAR");
    EnumPrettifier::Assign(SAMPLER_FILTER_NONE,    "SAMPLER_FILTER_NONE");

    EnumPrettifier::Assign(SAMPLER_REPEAT_MODE_REPEAT,               "SAMPLER_REPEAT_MODE_REPEAT");
    EnumPrettifier::Assign(SAMPLER_REPEAT_MODE_MIRRORED_REPEAT,      "SAMPLER_REPEAT_MODE_MIRRORED_REPEAT");
    EnumPrettifier::Assign(SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE,        "SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE");
    EnumPrettifier::Assign(SAMPLER_REPEAT_MODE_CLAMP_TO_BORDER,      "SAMPLER_REPEAT_MODE_CLAMP_TO_BORDER");
    EnumPrettifier::Assign(SAMPLER_REPEAT_MODE_MIRROR_CLAMP_TO_EDGE, "SAMPLER_REPEAT_MODE_MIRROR_CLAMP_TO_EDGE");

    return true;
}

void OpenGLRendererAPI::Shutdown() {}

void OpenGLRendererAPI::SetViewport(int XPosition, int YPosition, int Width, int Height)
{ SetViewport({XPosition, YPosition}, {Width, Height}); }

void OpenGLRendererAPI::SetViewport(Farg<Position2D> inPos, Farg<Size2D> inSize)
{ glViewport(inPos.x(), inPos.y(), inSize.w(), inSize.h()); }

void OpenGLRendererAPI::SetClearColor(double Red, double Green, double Blue, double Alpha)
{ mClearColor = {Red, Green, Blue, Alpha}; }

void OpenGLRendererAPI::SetClearColor(Farg<glm::vec4> Color)
{ mClearColor = {Color.r, Color.g, Color.b, Color.a}; }

void OpenGLRendererAPI::SetClearColor(Farg<ColorRGBA> Color)
{ mClearColor = {Color.r(), Color.g(), Color.b(), Color.a()}; }

void OpenGLRendererAPI::Clear()
{
    glClearColor(mClearColor[0],mClearColor[1],mClearColor[2],mClearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::SetLineWidth(float Width)
{ glLineWidth(Width); }

ID OpenGLRendererAPI::AddShader(Shared<Shader> inShader, ID inID)
{
    while(inID.invalid() or mShaders.contains(inID))
        { inID = UID::GetRandom(); }
    mShaders[inID] = inShader;
    return inID;
}

Shared<Shader> OpenGLRendererAPI::GetShader(ID inID)
{
    if(auto found_it{mShaders.find(inID[])}; found_it != mShaders.end())
        { return found_it->second; }
    return mShaders.at(Shaders::BlinnPhong);
}

Error OpenGLRendererAPI::RemoveShader(ID inID)
{
    return (mShaders.erase(inID))
        ? OK
        : (mShaders.empty())
            ? ERR_EMPTY
            : ERR_NOT_FOUND;
}

void OpenGLRendererAPI::SetLight_TempBlinnPhongSolution(Shared<Light3D> inLight)
{
#pragma message("Currently, lights are limited to a maximum number but that number isn't enforced in the code and can overflow")
    if(!inLight->IncrementIndex())
        { return; }
    std::string l_Light{std::format("_lights[{}].", inLight->Index())};
    switch(inLight->Type())
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
    case LightType::NONE:
        return;
    }

    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "color",             inLight->mColor);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "energy",            inLight->mEnergy);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "specular_strength", inLight->mSpecularStrength);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "ambient_strength",  inLight->mAmbientStrength);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "attenuation",       inLight->mAttenuation);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "range",             inLight->mRange);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "position",          inLight->Position());
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "direction",         inLight->Quaternion() * Settings::World::Front());
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "spot_cutoff",       glm::cos(glm::radians(inLight->mSpotAngle)));
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "spot_cutoff_fade",  glm::cos(glm::radians(inLight->mSpotAngle - inLight->mSpotAngleFade)));
}

void OpenGLRendererAPI::SetFramebufferSRGB(bool isOn) const
{
    if(isOn)
        { glEnable(GL_FRAMEBUFFER_SRGB); }
    else
        { glDisable(GL_FRAMEBUFFER_SRGB); }
}

void OpenGLRendererAPI::SetWireframe(bool isOn) const
{
    if(isOn)
        { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
    else
        { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
}

void OpenGLRendererAPI::SetBlend(bool isEnabled) const
{
    if(isEnabled) { glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); }
    else { glDisable(GL_BLEND); }
}

bool OpenGLRendererAPI::BindTexture(Shared<TextureBuffer> inBuffer, uint inUnit) const
{
    if(!inBuffer
        or !inBuffer->Status()
        or !inBuffer->ID())
            { return false; }
    glBindTextureUnit(inUnit, inBuffer->ID());
    return true;
}

bool OpenGLRendererAPI::BindTexture(Shared<Texture> inTexture, uint inUnit) const
{
    if(!inTexture->uid().invalid())
        { return BindTexture(inTexture->GetBuffer(), inUnit); }
    return false;
}

void OpenGLRendererAPI::UnbindTexture(texture_units inTextureUnits) const
{
    for(uint unit : inTextureUnits)
        { glBindTextureUnit(unit, 0); }
}

void OpenGLRendererAPI::DrawText(Sarg inText,
    Shared<Font> inFont, glm::vec2 inPos, glm::vec2 inScale)
{
    static uint VAO{0},
        VBO{0};

    if(!VAO or !VBO)
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 66, nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
    }

    glm::vec2 initial_position{inPos};
    inScale = {1.0f,1.0f};

    for(auto c{inText.cbegin()}; c != inText.cend(); ++c)
    {
        FAUTO glyph{inFont->GetGlyph(*c)};
        if(!glyph.texture) { continue; }

        glm::vec2 pos{
            inPos.x + glyph.bitmap_left * inScale.x,
            inPos.y - (glyph.bitmap_height - glyph.bitmap_top) * inScale.y,
        };

        glm::vec2 scale{
            inScale.x * glyph.bitmap_width,
            inScale.y * glyph.bitmap_height,
        };

        if(*c == '\n')
        {
            inPos.x  = initial_position.x;
            inPos.y -= glyph.bitmap_height * inScale.y;
            continue;
        }

        float vertices[66] =
        {
            pos.x          , pos.y + scale.y, 0, 1,1,1, 0,0,0, 0.0f, 1.0f,
            pos.x          , pos.y          , 0, 1,1,1, 0,0,0, 0.0f, 0.0f,
            pos.x + scale.x, pos.y          , 0, 1,1,1, 0,0,0, 1.0f, 0.0f,
            pos.x          , pos.y + scale.y, 0, 1,1,1, 0,0,0, 0.0f, 1.0f,
            pos.x + scale.x, pos.y          , 0, 1,1,1, 0,0,0, 1.0f, 0.0f,
            pos.x + scale.x, pos.y + scale.y, 0, 1,1,1, 0,0,0, 1.0f, 1.0f,
        };

        inPos.x += (glyph.advance_x >> 6) * inScale.x;
        inPos.y -= (glyph.advance_y >> 6) * inScale.y;

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindTextureUnit(0, glyph.texture->ID());
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void OpenGLRendererAPI::DrawIndexed(Shared<VertexArray> inVAO, uint inIndexCount)
{
    if(!inVAO) { return; }
    inVAO->Bind();
    inVAO->GetIndexBuffer()->Bind();
    glDrawElements(GL_TRIANGLES,
        (inIndexCount)
            ? inIndexCount
            : inVAO->GetIndexBuffer()->GetCount(),
        GL_UNSIGNED_INT,
        nullptr);
}

void OpenGLRendererAPI::DrawSkybox(Shared<VertexArray> inVAO)
{
    if(!inVAO) { return; }
    glDepthMask(GL_FALSE);
    inVAO->Bind();
    inVAO->GetIndexBuffer()->Bind();
    glDrawElements(GL_TRIANGLES,
        inVAO->GetIndexBuffer()->GetCount(),
        GL_UNSIGNED_INT,
        nullptr);
    inVAO->Unbind();
    glDepthMask(GL_TRUE);
}

//----------------------------------------------------------------------------------
// OpenGL Debug Message Callback Function
// https://gist.github.com/liam-middlebrook/c52b069e4be2d87a6d2f (with minor tweaks)
//----------------------------------------------------------------------------------
#ifdef NOSTALGIA_DEBUGGING
void APIENTRY OpenGL_DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* data)
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
        if(gOpenGLMessageFilter < DebugMessageSeverityFilter::High)
            { return; }
        _severity = "HIGH";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        if(gOpenGLMessageFilter < DebugMessageSeverityFilter::Medium)
            { return; }
        _severity = "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        if(gOpenGLMessageFilter < DebugMessageSeverityFilter::Low)
            { return; }
        _severity = "LOW";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        if(!gOpenGLEnableNotificationMesssages)
            { return; }
        _severity = "NOTIFICATION";
        break;
    default:
        if(gOpenGLMessageFilter == DebugMessageSeverityFilter::None)
            { return; }
        _severity = "UNKNOWN";
        break;
    }
    print_debug("{}: {} of {} severity, raised from {}: {}", id, _type, _severity, _source, message);
}

#endif // NOSTALGIA_DEBUGGING
