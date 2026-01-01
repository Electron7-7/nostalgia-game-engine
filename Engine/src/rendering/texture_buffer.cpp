#include "texture_buffer.hpp"
#include "renderer_api.hpp"
#include "core/printing.hpp"
#include "filesystem/file_data.hpp"
// Implementations
#include "backends/opengl/gl_texture_buffer.hpp"

Shared<TextureBuffer> TextureBuffer::Create(Farg<TextureFormat> inFormat, Farg<SamplerState> inSampler)
{ return Create(inFormat, inSampler, nullptr); }

Shared<TextureBuffer> TextureBuffer::Create(Farg<TextureFormat> inFormat, Farg<SamplerState> inSampler, Shared<FileData> inData)
{ return Create(inFormat, inSampler, cubemap_images_t{inData}); }

Shared<TextureBuffer> TextureBuffer::Create(Farg<TextureFormat> inFormat, Farg<SamplerState> inSampler, Farg<cubemap_images_t> inData)
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
        return MakeShared<OpenGLTextureBuffer>(inFormat, inSampler, inData);
    }
}
