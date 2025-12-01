#include "gl_renderer_api.hpp"
#include "core/printing.hpp"

#include <glad/glad.h>
#include <glm/vec4.hpp>

void OpenGLRendererAPI::Init()
{
    PRINT_PRETTY_FUNCTION;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
}

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
