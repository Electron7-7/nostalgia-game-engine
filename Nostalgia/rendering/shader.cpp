#include "shader.hpp"
#include "renderer_api.hpp"
// Implementations
#include "backends/opengl/gl_shader.hpp"

Shared<Shader> Shader::CreateDummy()
{ return MakeShared<DummyShader>(); }

Shared<Shader> Shader::Create()
{
    switch(RendererAPI::CurrentAPI())
    {
    case RendererAPI::NONE:
    default:
        print_warning(RendererAPI::s_cAPIWarningMessage);
        return CreateDummy();
    case RendererAPI::OPENGL:
        return MakeShared<GLShader>();
    }
}
