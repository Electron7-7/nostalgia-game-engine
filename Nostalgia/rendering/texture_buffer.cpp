#include "texture_buffer.hpp"
#include "renderer_api.hpp"
// Implementations
#include "backends/opengl/gl_texture_buffer.hpp"

Shared<TextureBuffer> TextureBuffer::Create(TextureType inType)
{
    switch(RendererAPI::CurrentAPI())
    {
    case RendererAPI::NONE:
    default:
        print_warning("RendererAPI is dummy; defaulting to OpenGL");
        [[fallthrough]];
    case RendererAPI::OPENGL:
        return MakeShared<OpenGLTextureBuffer>(inType);
    }
}
