#include "vertex_array.hpp"
#include "renderer_api.hpp"
#include "core/printing.hpp"

// Implementations
#include "backends/opengl/gl_vertex_array.hpp"

Shared<VertexArray> VertexArray::Create()
{
    std::string error_api_name{"GraphicsAPI::None"};
    switch(IRendererAPI::GetAPI())
    {
    default:
        error_api_name = "Invalid";
        [[fallthrough]];
    case GraphicsAPI::None:
        print_warning("IRendererAPI::GetAPI() returned '{}' (defaulting to OpenGL)", error_api_name);
        [[fallthrough]];
    case GraphicsAPI::OpenGL:
        return MakeShared<OpenGLVertexArray>();
    }
}
