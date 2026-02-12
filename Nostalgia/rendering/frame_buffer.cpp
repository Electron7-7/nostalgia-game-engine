#include "frame_buffer.hpp"
#include "renderer_api.hpp"
// Implementations
#include "backends/opengl/gl_frame_buffer.hpp"

Shared<FrameBuffer> FrameBuffer::Create()
{ return Create({0,0}); }

Shared<FrameBuffer> FrameBuffer::Create(Farg<Size2D> inScale)
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
        if(inScale == Size2D{0,0})
            { return MakeShared<OpenGLFrameBuffer>(); }
        else
            { return MakeShared<OpenGLFrameBuffer>(inScale); }
    }
}
