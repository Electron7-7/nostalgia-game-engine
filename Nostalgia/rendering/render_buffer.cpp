#include "./render_buffer.hpp"
#include "./renderer_api.hpp"
// Implementations
#include "backends/opengl/gl_render_buffer.hpp"

Shared<RenderBuffer> RenderBuffer::Create()
{
    switch(RendererAPI::CurrentAPI())
    {
    case RendererAPI::NONE:
    default:
        print_warning("RendererAPI is dummy; defaulting to OpenGL");
        [[fallthrough]];
    case RendererAPI::OPENGL:
        return MakeShared<OpenGLRenderBuffer>();
    }
}

Shared<RenderBuffer> RenderBuffer::Create(Farg<Size2D> inSize, DataFormat inComponent)
{
    auto _output{Create()};
    _output->SetStorage(inSize, inComponent);
    return _output;
}
