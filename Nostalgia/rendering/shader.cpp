#include "shader.hpp"
#include "renderer_api.hpp"
// Implementations
#include "backends/opengl/gl_shader.hpp"

Shared<Shader> Shader::Create()
{
    switch(RendererAPI::CurrentAPI())
    {
    case RendererAPI::NONE:
    default:
        print_warning("RendererAPI is dummy; defaulting to OpenGL");
        [[fallthrough]];
    case RendererAPI::OPENGL:
        return MakeShared<GLShader>();
    }
}
