#include "./vertex_array.hpp"
#include "./renderer_api.hpp"
// Implementations
#include "backends/opengl/gl_vertex_array.hpp"

Shared<VertexArray> VertexArray::CreateDummy()
{ return MakeShared<DummyVertexArray>(); }

Shared<VertexArray> VertexArray::Create()
{
    switch(RendererAPI::CurrentAPI())
    {
    case RendererAPI::NONE:
    default:
        print_warning(RendererAPI::s_cAPIWarningMessage);
        return CreateDummy();
    case RendererAPI::OPENGL:
        return MakeShared<OpenGLVertexArray>();
    }
}
