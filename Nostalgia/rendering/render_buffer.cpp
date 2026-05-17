#include "./render_buffer.hpp"
#include "./renderer_api.hpp"
// Implementations
#include "backends/opengl/gl_render_buffer.hpp"

Shared<RenderBuffer> RenderBuffer::CreateDummy()
{ return MakeShared<DummyRenderBuffer>(); }

Shared<RenderBuffer> RenderBuffer::Create()
{
    switch(RendererAPI::CurrentAPI())
    {
    case RendererAPI::NONE:
    default:
        print_warning(RendererAPI::s_cAPIWarningMessage);
        return RenderBuffer::CreateDummy();
    case RendererAPI::OPENGL:
        return MakeShared<OpenGLRenderBuffer>();
    }
}

Shared<RenderBuffer> RenderBuffer::Create(Farg<Size2D<int>> inSize, DataFormat inComponent)
{
    auto _output{Create()};
    _output->SetStorage(inSize, inComponent);
    return _output;
}
