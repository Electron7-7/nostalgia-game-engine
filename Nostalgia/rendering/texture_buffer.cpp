#include "texture_buffer.hpp"
#include "renderer_api.hpp"
// Implementations
#include "backends/opengl/gl_texture_buffer.hpp"

Shared<TextureBuffer> TextureBuffer::CreateDummy()
{ return MakeShared<DummyTextureBuffer>(); }

Shared<TextureBuffer> TextureBuffer::Create(Farg<TextureFormat> inFormat)
{
    switch(RendererAPI::CurrentAPI())
    {
    case RendererAPI::NONE:
    default:
        print_warning(RendererAPI::s_cAPIWarningMessage);
        return TextureBuffer::CreateDummy();
    case RendererAPI::OPENGL:
        return MakeShared<OpenGLTextureBuffer>(inFormat);
    }
}
