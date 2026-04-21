#include "gl_renderer_api.hpp"
#include "gl_shader.hpp"        // IWYU pragma: keep // clangd crashes when processing the embedded shaders so I hide them from it
#include "shaders.hpp" // IWYU pragma: keep // clangd crashes when processing the embedded shaders so I hide them from it
#include "things/resources/image.hpp"
#include "things/resources/texture.hpp"
#include "things/resources/font.hpp"
#include "things/thinkers/3d/light_3d.hpp"
#include "application/application.hpp"
#include "rendering/debugging.hpp"
#include "rendering/texture_buffer.hpp" // IWYU pragma: keep // idk why clangd says these are unused
#include "rendering/vertex_array.hpp"   // IWYU pragma: keep // idk why clangd says these are unused
#include "rendering/frame_buffer.hpp"   // IWYU pragma: keep // idk why clangd says these are unused
#include "settings/world.hpp"
#include "thirdparty/glad/glad.h"

#define LOCK_MUTEX(MUTEX) LockGuard<RMutex> MUTEX##_lock{MUTEX};

bool gPrintDrawLogs{false};
bool gOpenGLEnableNotificationMesssages{false};
DebugMessageSeverityFilter gOpenGLMessageFilter{DebugMessageSeverityFilter::High};

static bool sWireframe{};
static constinit const float TEXT_UVS[12]{0,1, 0,0, 1,0, 0,1, 1,0, 1,1};

#ifdef NOSTALGIA_DEBUGGING
static void APIENTRY OpenGL_DebugMessageCallback(GLenum,GLenum,GLuint,GLenum,GLsizei,const GLchar*,const void*);
#endif // NOSTALGIA_DEBUGGING

bool OpenGLRendererAPI::Init()
{
    if(mIsRunning)
        { return true; }

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

    GLintptr _offsets[2]{0, 0};
    GLsizei  _strides[2]{2 * sizeof(float), 2 * sizeof(float)};

    glCreateVertexArrays(1, &mTextVAO);
    glCreateBuffers(2, mTextVBOs);
    glNamedBufferData(mTextVBOs[0], 12 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glNamedBufferData(mTextVBOs[1], 12 * sizeof(float), TEXT_UVS, GL_STATIC_DRAW);
    glVertexArrayVertexBuffers(mTextVAO, 0, 2, mTextVBOs, _offsets, _strides);
    glEnableVertexArrayAttrib(mTextVAO, 0);
    glEnableVertexArrayAttrib(mTextVAO, 1);
    glVertexArrayAttribFormat(mTextVAO, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(mTextVAO, 1, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(mTextVAO, 0, 0);
    glVertexArrayAttribBinding(mTextVAO, 1, 1);

    EnumRegistry::Assign(TEXTURE_TYPE_1D,         "TEXTURE_TYPE_1D");
    EnumRegistry::Assign(TEXTURE_TYPE_2D,         "TEXTURE_TYPE_2D");
    EnumRegistry::Assign(TEXTURE_TYPE_3D,         "TEXTURE_TYPE_3D");
    EnumRegistry::Assign(TEXTURE_TYPE_1D_ARRAY,   "TEXTURE_TYPE_1D_ARRAY");
    EnumRegistry::Assign(TEXTURE_TYPE_2D_ARRAY,   "TEXTURE_TYPE_2D_ARRAY");
    EnumRegistry::Assign(TEXTURE_TYPE_3D_ARRAY,   "TEXTURE_TYPE_3D_ARRAY");
    EnumRegistry::Assign(TEXTURE_TYPE_CUBE,       "TEXTURE_TYPE_CUBE");
    EnumRegistry::Assign(TEXTURE_TYPE_CUBE_ARRAY, "TEXTURE_TYPE_CUBE_ARRAY");
    EnumRegistry::Assign(TEXTURE_TYPE_NONE,       "TEXTURE_TYPE_NONE");

    EnumRegistry::Assign(SAMPLER_FILTER_NEAREST, "SAMPLER_FILTER_NEAREST");
    EnumRegistry::Assign(SAMPLER_FILTER_LINEAR,  "SAMPLER_FILTER_LINEAR");
    EnumRegistry::Assign(SAMPLER_FILTER_NONE,    "SAMPLER_FILTER_NONE");

    EnumRegistry::Assign(SAMPLER_REPEAT_MODE_REPEAT,               "SAMPLER_REPEAT_MODE_REPEAT");
    EnumRegistry::Assign(SAMPLER_REPEAT_MODE_MIRRORED_REPEAT,      "SAMPLER_REPEAT_MODE_MIRRORED_REPEAT");
    EnumRegistry::Assign(SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE,        "SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE");
    EnumRegistry::Assign(SAMPLER_REPEAT_MODE_CLAMP_TO_BORDER,      "SAMPLER_REPEAT_MODE_CLAMP_TO_BORDER");
    EnumRegistry::Assign(SAMPLER_REPEAT_MODE_MIRROR_CLAMP_TO_EDGE, "SAMPLER_REPEAT_MODE_MIRROR_CLAMP_TO_EDGE");

    return mIsRunning = true;
}

void OpenGLRendererAPI::Shutdown()
{
    LOCK_MUTEX(mShadersMutex)
    LOCK_MUTEX(mTextMutex)
    mIsRunning = false;
    mShaders.clear();
    glDeleteBuffers(2, mTextVBOs);
    glDeleteVertexArrays(1, &mTextVAO);
}

bool OpenGLRendererAPI::IsRunning()
{ return mIsRunning; }

void OpenGLRendererAPI::SetViewport(int XPosition, int YPosition, int Width, int Height)
{ SetViewport({XPosition, YPosition}, {Width, Height}); }

void OpenGLRendererAPI::SetViewport(Farg<Position2D> inPos, Farg<Size2D> inSize)
{ glViewport(inPos.x(), inPos.y(), inSize.w(), inSize.h()); }

void OpenGLRendererAPI::SetClearColor(Farg<ColorRGBA> Color)
{ glClearColor(Color.r(), Color.g(), Color.b(), Color.a()); }

void OpenGLRendererAPI::Clear()
{ glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void OpenGLRendererAPI::SetLineWidth(float Width)
{ glLineWidth(Width); }

ID OpenGLRendererAPI::AddShader(Shared<Shader> inShader, ID inID)
{
    LOCK_MUTEX(mShadersMutex)
    while(inID.invalid() or mShaders.contains(inID))
        { inID = inID() + 1; }
    mShaders[inID] = inShader;
    return inID;
}

Shared<Shader> OpenGLRendererAPI::GetShader(ID inID)
{
    LOCK_MUTEX(mShadersMutex)
    if(auto found_it{mShaders.find(inID())}; found_it != mShaders.end())
        { return found_it->second; }
    return mShaders.at(Shaders::BlinnPhong);
}

Error OpenGLRendererAPI::RemoveShader(ID inID)
{
    LOCK_MUTEX(mShadersMutex)
    return (mShaders.erase(inID))
        ? OK
        : (mShaders.empty())
            ? ERR_EMPTY
            : ERR_NOT_FOUND;
}

void OpenGLRendererAPI::SetLight_TempBlinnPhongSolution(Shared<Light3D> inLight)
{
#pragma message("Currently, lights are limited to a maximum number but that number isn't enforced in the code and can overflow")
    if(not inLight->IncrementIndex())
        { return; }
    std::string l_Light{std::format("_lights[{}].", inLight->Index())};
    if(inLight->DerivedFrom(ThingType::PointLight3D))
        { l_Light = "point" + l_Light; }
    else if(inLight->DerivedFrom(ThingType::SpotLight3D))
        { l_Light = "spot" + l_Light; }
    else if(inLight->DerivedFrom(ThingType::DirectionalLight3D))
        { l_Light = "directional" + l_Light; }
    else
        { return; }

    LOCK_MUTEX(mShadersMutex)
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "color",             inLight->mColor);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "energy",            inLight->mEnergy);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "specular_strength", inLight->mSpecularStrength);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "ambient_strength",  inLight->mAmbientStrength);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "attenuation",       inLight->mAttenuation);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "range",             inLight->mRange);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "position",          inLight->GlobalPosition());
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "direction",         glm::quat{inLight->GlobalRotation()} * Settings::World::Front());
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
    sWireframe = isOn;
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

void OpenGLRendererAPI::SetDepthMask(bool isEnabled) const
{ glDepthMask((isEnabled) ? GL_TRUE : GL_FALSE); }

ColorRGBA OpenGLRendererAPI::GetClearColor()
{
    ColorRGBA _output{};
    glGetFloatv(GL_COLOR_CLEAR_VALUE, _output.data());
    return _output;
}

float OpenGLRendererAPI::GetLineWidth()
{
    float output;
    glGetFloatv(GL_LINE_WIDTH, &output);
    return output;
}

bool OpenGLRendererAPI::GetWireframe() const
{ return sWireframe; }

bool OpenGLRendererAPI::GetBlend() const
{
    unsigned char output;
    glGetBooleanv(GL_BLEND, &output);
    return static_cast<bool>(output);
}

// See [https://www.songho.ca/opengl/gl_pbo.html#pack] for examples on PBO usage
#pragma message("TODO: use PBOs for screenshots")
Shared<Image> OpenGLRendererAPI::GetFullScreenshot() const
{
    // https://lencerf.github.io/post/2019-09-21-save-the-opengl-rendering-to-image-file
    auto _size{MainWindow()->GetScale()};
    GLsizei _width{_size.w()},
        _height{_size.h()},
        _channels{4};
    std::vector<uchar> _buffer(_width * _height * _channels);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadPixels(0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, _buffer.data());
    return Image::CreateFromData(_buffer.data(), _buffer.size(), _width, _height, _channels, true,
        DATA_FORMAT_RGBA);
}

bool OpenGLRendererAPI::BindTexture(Shared<TextureBuffer> inBuffer, uint inUnit) const
{
    if(not inBuffer
        or not inBuffer->ID()
        or not inBuffer->Status())
            { return false; }
    glBindTextureUnit(inUnit, inBuffer->ID());
    return true;
}

bool OpenGLRendererAPI::BindTexture(Shared<Texture> inTexture, uint inUnit) const
{
    if(not inTexture)
        { return false; }
    return BindTexture(inTexture->GetBuffer(), inUnit);
}

void OpenGLRendererAPI::UnbindTexture(texture_units inTextureUnits) const
{
    for(uint unit : inTextureUnits)
        { glBindTextureUnit(unit, 0); }
}

void OpenGLRendererAPI::DrawText(Sarg inText,
    Shared<Font> inFont, glm::vec2 inPos, Farg<glm::vec2> inScale)
{
    if(not inFont or not mTextVAO)
        { return; }

    LOCK_MUTEX(mTextMutex)

    glBindVertexArray(mTextVAO);

    glm::vec2 initial_position{inPos};
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

        float _positions[12]
        {
            pos.x,           pos.y + scale.y,
            pos.x,           pos.y,
            pos.x + scale.x, pos.y,
            pos.x,           pos.y + scale.y,
            pos.x + scale.x, pos.y,
            pos.x + scale.x, pos.y + scale.y
        };

        inPos.x += (glyph.advance_x >> 6) * inScale.x;
        inPos.y -= (glyph.advance_y >> 6) * inScale.y;

        glBindTextureUnit(0, glyph.texture->ID());
        glNamedBufferSubData(mTextVBOs[0], 0, 12 * sizeof(float), _positions);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void OpenGLRendererAPI::DrawIndexed(Shared<VertexArray> inVAO, uint inIndexCount, uint inIndexOffset)
{
    if(not inVAO or (not inIndexCount and not inVAO->GetIndexBuffer()))
        { return; }
    inIndexCount = (inIndexCount) ? inIndexCount : inVAO->GetIndexBuffer()->GetCount();
    inVAO->Bind();
    glDrawElementsBaseVertex(GL_TRIANGLES, inIndexCount, GL_UNSIGNED_INT, nullptr, inIndexOffset);
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
