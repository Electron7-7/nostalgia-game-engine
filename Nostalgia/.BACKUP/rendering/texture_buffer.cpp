#include "./texture_buffer.hpp"
#include "./renderer_api.hpp"
#include "things/thing_data.hpp"
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

void SamplerState::SetVariables(Farg<TheatreFile::ThingData> inData)
{
    inData.get_variable(min_filter, "SamplerMinFilter", "MinFilter", "Min");
    inData.get_variable(mip_filter_min, "SamplerMipFilterMin", "MipFilterMin", "MipMin");
    inData.get_variable(mag_filter, "SamplerMagFilter", "MagFilter", "Mag");
    inData.get_variable(repeat_u, "SamplerRepeatU", "SamplerRepeatX", "RepeatU", "RepeatX");
    inData.get_variable(repeat_v, "SamplerRepeatV", "SamplerRepeatY", "RepeatV", "RepeatY");
    inData.get_variable(repeat_w, "SamplerRepeatW", "SamplerRepeatZ", "RepeatW", "RepeatZ");
    inData.get_variable(use_anisotropy, "UseAnisotropy", "AnisotropyEnabled");
    inData.get_variable(anisotropy_max, "AnisotropyMax", "Anisotropy");
}

void SamplerState::GetVariables(Shared<TheatreFile::ThingData>& ioData) const
{
    ioData->set_variable(min_filter, "SamplerMinFilter");
    ioData->set_variable(mip_filter_min, "SamplerMipFilterMin");
    ioData->set_variable(mag_filter, "SamplerMagFilter");
    ioData->set_variable(repeat_u, "SamplerRepeatU");
    ioData->set_variable(repeat_v, "SamplerRepeatV");
    ioData->set_variable(repeat_w, "SamplerRepeatW");
    ioData->set_variable(use_anisotropy, "UseAnisotropy");
    ioData->set_variable(anisotropy_max, "AnisotropyMax");
}
