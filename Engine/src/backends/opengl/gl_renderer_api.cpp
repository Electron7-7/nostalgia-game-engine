#include "gl_renderer_api.hpp"
#include "gl_shader.hpp"        // IWYU pragma: keep
#include "embedded/shaders.hpp" // IWYU pragma: keep
#include "core/printing.hpp"
#include "rendering/vertex_array.hpp"
#include "rendering/frame_buffer.hpp"
#include "things/actors/light.hpp"
#include "application/application.hpp"

#include <glad/glad.h>
#include <glm/vec4.hpp>
#include <glm/ext/matrix_clip_space.hpp>

bool gGLViewportUseDirectFromViewport{false};
bool gPrintDrawLogs{false};
bool gPrintViewportAddedLogs{false};

bool OpenGLRendererAPI::Init()
{
    PRINT_PRETTY_FUNCTION;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);

    mShaders[Shaders::BlinnPhong] = Shader::Create();
    mShaders[Shaders::Fullbright] = Shader::Create();

    mViewports[Viewport::IDs::MainWindow] = Viewport{MainWindow()->GetScale(), MainWindow()->GetPosition(), 31};

    mFramebuffers[FrameBuffer::IDs::Default] = FrameBuffer::Create();

#ifndef CLANGD_KEEPS_CRASHING_HERE
    mShaders[Shaders::BlinnPhong]->CompileShader(GLSL_BlinnPhong_Vert, GLSL_BlinnPhong_Frag);
    mShaders[Shaders::Fullbright]->CompileShader(GLSL_BlinnPhong_Vert, GLSL_FullBright_Frag);
#endif
    return true;
}

void OpenGLRendererAPI::Shutdown()
{ mFramebuffers.clear(); }

void OpenGLRendererAPI::SetViewport(uint XPosition, uint YPosition, uint Width, uint Height)
{ glViewport(XPosition, YPosition, Width, Height); }

void OpenGLRendererAPI::SetClearColor(float Red, float Green, float Blue, float Alpha)
{ glClearColor(Red, Green, Blue, Alpha); }

void OpenGLRendererAPI::SetClearColor(const glm::vec4& Color)
{ glClearColor(Color.r, Color.g, Color.b, Color.a); }

void OpenGLRendererAPI::Clear()
{ glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

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

ID OpenGLRendererAPI::AddViewport(Farg<Viewport> inViewport, ID inID)
{
    while(inID.invalid() or mViewports.contains(inID))
        { inID = ID::Generate(); }
    mViewports[inID] = inViewport;
    if(gPrintViewportAddedLogs)
        { print_debug("Added new Viewport with ID#{}", inID[]); }
    return inID;
}

Viewport& OpenGLRendererAPI::GetViewport(ID inID)
{
    if(auto found_it{mViewports.find(inID)}; found_it != mViewports.end())
        { return found_it->second; }
    return mViewports.at(Viewport::IDs::MainWindow);
}

Error OpenGLRendererAPI::RemoveViewport(ID inID)
{
    if(inID == Viewport::IDs::MainWindow)
        { return ERR_NOT_ALLOWED; }
    return (mViewports.erase(inID))
        ? OK
        : ERR_NOT_FOUND;
}

ID OpenGLRendererAPI::GenerateFrameBuffer(ID inID)
{
    while(inID.invalid() or mFramebuffers.contains(inID))
        { inID = ID::Generate(); }
    mFramebuffers[inID] = FrameBuffer::Create();
    mFramebuffers[inID]->Generate();
    if(gPrintViewportAddedLogs)
        { print_debug("Added new FrameBuffer with ID#{}", inID[]); }
    return inID;
}

Shared<FrameBuffer>& OpenGLRendererAPI::GetFrameBuffer(ID inID)
{
    if(auto found_it{mFramebuffers.find(inID)}; found_it != mFramebuffers.end())
        { return found_it->second; }
    return mFramebuffers[FrameBuffer::IDs::Default];
}

Error OpenGLRendererAPI::RemoveFrameBuffer(ID inID)
{
    if(inID == FrameBuffer::IDs::Default)
        { return ERR_NOT_ALLOWED; }
    return (mFramebuffers.erase(inID))
        ? OK
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
    for(FAUTO [id, viewport] : mViewports)
    {
        if(viewport.layers.contains(inLayers))
        {
            glViewport(static_cast<int>(viewport.position.x()),
                static_cast<int>(viewport.position.y()),
                static_cast<int>(viewport.scale.w()),
                static_cast<int>(viewport.scale.h()));
            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
            if(gPrintDrawLogs)
            {
                print_debug("glDrawElements called w/ Viewport#{} (pos: {}, scale: {}, layers: {:#b})",
                    id[],
                    viewport.position.data_log(),
                    viewport.scale.data_log(),
                    viewport.layers.get());
            }
        }
    }
    for(FAUTO [id, framebuffer] : mFramebuffers)
    {
        framebuffer->Bind();
        glViewport(0,0,
            static_cast<int>(framebuffer->GetScale().w()),
            static_cast<int>(framebuffer->GetScale().h()));
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
        framebuffer->Unbind();
    }
}

void OpenGLRendererAPI::DrawLines(Shared<VertexArray> inVertexArray, uint inVertexCount, RenderLayers inLayers)
{
    inVertexArray->Bind();
    for(FAUTO [id, framebuffer] : mFramebuffers)
    {
        framebuffer->Bind();
        glViewport(0,0,
            static_cast<int>(framebuffer->GetScale().w()),
            static_cast<int>(framebuffer->GetScale().h()));
        glDrawArrays(GL_LINES, 0, inVertexCount);
        framebuffer->Unbind();
    }
    for(FAUTO [id, viewport] : mViewports)
    {
        if(viewport.layers.contains(inLayers))
        {
            glViewport(static_cast<int>(viewport.position.x()),
                static_cast<int>(viewport.position.y()),
                static_cast<int>(viewport.scale.w()),
                static_cast<int>(viewport.scale.h()));
            glDrawArrays(GL_LINES, 0, inVertexCount);
            if(gPrintDrawLogs)
            {
                print_debug("glDrawElements called w/ Viewport#{} (pos: {}, scale: {}, layers: {:#b})",
                    id[],
                    viewport.position.data_log(),
                    viewport.scale.data_log(),
                    viewport.layers.get());
            }
        }
    }
}
