#include "shader.hpp"
#include "renderer_api.hpp"
#include "core/printing.hpp"
// Implementations
#include "backends/opengl/gl_shader.hpp"

Shared<Shader> Shader::Create()
{
    std::string error_api_name{"GraphicsAPI::None"};
    switch(RendererAPI::GetAPI())
    {
    default:
        error_api_name = "Invalid";
        [[fallthrough]];
    case GraphicsAPI::None:
        print_warning("RendererAPI::GetAPI() returned '{}' (defaulting to OpenGL)", error_api_name);
        [[fallthrough]];
    case GraphicsAPI::OpenGL:
        return MakeShared<GLShader>();
    }
}
