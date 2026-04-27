#include "./render_buffer.hpp"
#include "./renderer_api.hpp"
// Implementations
#include "backends/opengl/gl_render_buffer.hpp"

Shared<RenderBuffer> RenderBuffer::Create()
{ return MakeShared<OpenGLRenderBuffer>(); }

Shared<RenderBuffer> RenderBuffer::Create(Farg<Size2D> inSize, DataFormat inComponent)
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
        return MakeShared<OpenGLRenderBuffer>(inSize, inComponent);
    }
}

Shared<RenderBuffer> RenderBuffer::Create(Farg<Size2D> inSize, DataFormat inComponent)
{
    auto _output{Create()};
    _output->SetStorage(inSize, inComponent);
    return _output;
}
