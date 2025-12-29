#include "gl_renderer_api.hpp"
#include "gl_shader.hpp"        // IWYU pragma: keep // clangd crashes when processing the embedded shaders so I hide them from it
#include "embedded/shaders.hpp" // IWYU pragma: keep // clangd crashes when processing the embedded shaders so I hide them from it
#include "core/printing.hpp"
#include "rendering/vertex_array.hpp"
#include "rendering/frame_buffer.hpp"
#include "things/actors/light.hpp"
#include "application/application.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <glm/ext/matrix_clip_space.hpp>

bool gPrintDrawLogs{false};
DebugMessageSeverityFilter gOpenGLMessageFilter{DebugMessageSeverityFilter::None};

static void APIENTRY OpenGL_DebugMessageCallback(GLenum,GLenum,GLuint,GLenum,GLsizei,const GLchar*,const void*);

bool OpenGLRendererAPI::Init()
{
    PRINT_PRETTY_FUNCTION;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);

#ifdef DEBUGGING
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(OpenGL_DebugMessageCallback, nullptr);
#endif

    mShaders[Shaders::BlinnPhong] = Shader::Create();
    mShaders[Shaders::Fullbright] = Shader::Create();

    SetViewport({0, 0}, MainWindow()->GetScale());

#ifndef CLANGD_KEEPS_CRASHING_HERE
    mShaders[Shaders::BlinnPhong]->CompileShader(GLSL_BlinnPhong_Vert, GLSL_BlinnPhong_Frag);
    mShaders[Shaders::Fullbright]->CompileShader(GLSL_BlinnPhong_Vert, GLSL_FullBright_Frag);
#endif
    return true;
}

void OpenGLRendererAPI::Shutdown()
{ mFramebuffers.clear(); }

void OpenGLRendererAPI::SetViewport(int XPosition, int YPosition, int Width, int Height)
{ SetViewport({XPosition, YPosition}, {Width, Height}); }

void OpenGLRendererAPI::SetViewport(Farg<Position2D> inPos, Farg<Scale2D> inSize)
{
    mViewportSize = inSize;
    mViewportPosition = inPos;
    glViewport(inPos.x(), inPos.y(), inSize.w(), inSize.h());
}

void OpenGLRendererAPI::SetClearColor(float Red, float Green, float Blue, float Alpha)
{ mClearColor = {Red, Green, Blue, Alpha}; }

void OpenGLRendererAPI::SetClearColor(const glm::vec4& Color)
{ mClearColor = {Color.r, Color.g, Color.b, Color.a}; }

void OpenGLRendererAPI::Clear()
{
    glClearColor(mClearColor[0],mClearColor[1],mClearColor[2],mClearColor[3]);
    for(auto& [id, framebuffer] : mFramebuffers)
    {
        framebuffer->Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        framebuffer->Unbind();
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::SetLineWidth(float Width)
{ glLineWidth(Width); }

ID OpenGLRendererAPI::AddShader(Shared<Shader> inShader, ID inID)
{
    while(inID.invalid() or mShaders.contains(inID))
        { inID = ID::Generate(); }
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

ID OpenGLRendererAPI::AddFrameBuffer(Shared<FrameBuffer> inFrameBuffer, ID inID)
{
    while(inID.invalid() or mFramebuffers.contains(inID))
        { inID = ID::Generate(); }
    mFramebuffers[inID] = inFrameBuffer;
    return inID;
}

Shared<FrameBuffer> OpenGLRendererAPI::GetFrameBuffer(ID inID)
{
    if(auto found_it{mFramebuffers.find(inID)}; found_it != mFramebuffers.end())
        { return found_it->second; }
    print_warning("Unable to find FrameBuffer#{}", inID[]);
    return FrameBuffer::Create({});
}

Error OpenGLRendererAPI::RemoveFrameBuffer(ID inID)
{
    return (mFramebuffers.erase(inID))
        ? OK
        : (mFramebuffers.empty())
            ? ERR_EMPTY
            : ERR_NOT_FOUND;
}

void OpenGLRendererAPI::SetLight_TempBlinnPhongSolution(light_t* inLight)
{
#pragma message("Currently, lights are limited to a maximum number but that number isn't enforced in the code and can overflow")
    std::string l_Light = std::format("_lights[{}].", inLight->Index());
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
    }

    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "color",             inLight->mColor);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "energy",            inLight->mEnergy);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "specular_strength", inLight->mSpecularStrength);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "ambient_strength",  inLight->mAmbientStrength);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "attenuation",       inLight->mAttenuation);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "range",             inLight->mRange);
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "position",          inLight->Origin());
    GetShader(Shaders::BlinnPhong)->SetUniform(l_Light + "direction",         inLight->Front());
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

void OpenGLRendererAPI::DrawIndexed(Shared<VertexArray> inVertexArray, uint inIndexCount, RenderLayers inLayers)
{
    inVertexArray->Bind();
    inVertexArray->GetIndexBuffer()->Bind();
    uint count{(inIndexCount) ? inIndexCount : inVertexArray->GetIndexBuffer()->GetCount()};

    for(FAUTO [id, framebuffer] : mFramebuffers)
    {
        framebuffer->Bind();
        glViewport(0, 0, framebuffer->TextureSize().w(), framebuffer->TextureSize().h());
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
        framebuffer->Unbind();
        if(gPrintDrawLogs)
        {
            print_debug("glDrawElements called w/ FrameBuffer#{} (size: {})",
                id[],
                framebuffer->TextureSize().data_log());
        }
    }
    SetViewport(mViewportPosition, mViewportSize);
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    if(gPrintDrawLogs)
    {
        print_debug("glDrawElements called (viewport size: {}, viewport position: {})",
            mViewportSize.data_log(),
            mViewportPosition.data_log());
    }
}

void OpenGLRendererAPI::DrawLines(Shared<VertexArray> inVertexArray, uint inVertexCount, RenderLayers inLayers)
{
    inVertexArray->Bind();
    for(FAUTO [id, framebuffer] : mFramebuffers)
    {
        framebuffer->Bind();
        glViewport(0, 0, framebuffer->TextureSize().w(), framebuffer->TextureSize().h());
        glDrawArrays(GL_LINES, 0, inVertexCount);
        framebuffer->Unbind();
        if(gPrintDrawLogs)
        {
            print_debug("glDrawArrays called w/ FrameBuffer#{} (size: {})",
                id[],
                framebuffer->TextureSize().data_log());
        }
    }
    SetViewport(mViewportPosition, mViewportSize);
    glDrawArrays(GL_LINES, 0, inVertexCount);
    if(gPrintDrawLogs)
    {
        print_debug("glDrawArrays called (viewport size: {}, viewport position: {})",
            mViewportSize.data_log(),
            mViewportPosition.data_log());
    }
}

//----------------------------------------------------------------------------------
// OpenGL Debug Message Callback Function
// https://gist.github.com/liam-middlebrook/c52b069e4be2d87a6d2f (with minor tweaks)
//----------------------------------------------------------------------------------
#ifdef DEBUGGING
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
        if(gOpenGLMessageFilter < DebugMessageSeverityFilter::Notification)
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

#else // !DEBUGGING
void APIENTRY OpenGL_DebugMessageCallback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const void*) {}
#endif // DEBUGGING
