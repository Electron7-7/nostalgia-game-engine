#include "frame_buffer.hpp"
#include "renderer_api.hpp"
#include "core/printing.hpp"
#include "application/application.hpp"

// Implementations
#include "backends/opengl/gl_frame_buffer.hpp"

Shared<FrameBuffer> FrameBuffer::Create()
{ return Create(MainWindow()->GetScale()); }

Shared<FrameBuffer> FrameBuffer::Create(Farg<Scale2D> inScale)
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
        return MakeShared<OpenGLFrameBuffer>(inScale);
    }
}
