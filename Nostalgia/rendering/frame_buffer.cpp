#include "frame_buffer.hpp"
#include "renderer_api.hpp"
// Implementations
#include "backends/opengl/gl_frame_buffer.hpp"

Shared<FrameBuffer> FrameBuffer::Create()
{
    switch(RendererAPI::CurrentAPI())
    {
    case RendererAPI::NONE:
    default:
        print_warning(RendererAPI::s_cAPIWarningMessage);
        [[fallthrough]];
    case RendererAPI::OPENGL:
        return MakeShared<OpenGLFrameBuffer>();
    }
}
