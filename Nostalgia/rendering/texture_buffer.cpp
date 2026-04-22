#include "texture_buffer.hpp"
#include "renderer_api.hpp"
// Implementations
#include "backends/opengl/gl_texture_buffer.hpp"

Shared<TextureBuffer> TextureBuffer::Create(TextureType inType)
{
    std::string error_api_name{"GraphicsAPI::None"};
    switch(RendererAPI::GetAPI())
    {
    default:
        error_api_name = "Invalid";
        [[fallthrough]];
    case GraphicsAPI::None:
        print_warning("RendererAPI::GetAPI() returned '{}' (defaulting to OpenGL)",
            error_api_name);
        [[fallthrough]];
    case GraphicsAPI::OpenGL:
        return MakeShared<OpenGLTextureBuffer>(inType);
    }
}
