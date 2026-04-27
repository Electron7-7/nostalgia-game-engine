#include "./vertex_array.hpp"
#include "./renderer_api.hpp"
// Implementations
#include "backends/opengl/gl_vertex_array.hpp"

Shared<VertexArray> VertexArray::Create()
{
    switch(RendererAPI::CurrentAPI())
    {
    case RendererAPI::NONE:
    default:
        print_warning("RendererAPI is dummy; defaulting to OpenGL");
        [[fallthrough]];
    case RendererAPI::OPENGL:
        return MakeShared<OpenGLVertexArray>();
    }
}
