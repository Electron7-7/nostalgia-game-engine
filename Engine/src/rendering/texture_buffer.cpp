#include "texture_buffer.hpp"
#include "renderer_api.hpp"
#include "core/printing.hpp"
#include "filesystem/file_data.hpp"
// Implementations
#include "backends/opengl/gl_texture_buffer.hpp"

Shared<TextureBuffer> TextureBuffer::Create(Farg<TextureFormat> inFormat, Shared<FileData> inData)
{ return Create(inFormat, {}, inData); }

Shared<TextureBuffer> TextureBuffer::Create(Farg<TextureFormat> inFormat, Farg<SamplerState> inSamplerState, Shared<FileData> inData)
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
        return MakeShared<OpenGLTextureBuffer>(inFormat, inSamplerState, inData);
    }
}
