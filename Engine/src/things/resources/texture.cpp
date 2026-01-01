#include "texture.hpp"
#include "core/uid.hpp"
#include "rendering/texture_buffer.hpp"
#include "theatre/parser/thing_data.hpp"

void Texture::Ready()
{
    if(UID::IsReserved(mUID[]))
        { m_pFileData = FileData::s_GetReservedFileData(mUID[]); }
    mTextureBuffer = TextureBuffer::Create(mFormat, mSampler, m_pFileData);
    if(!print_error_enum(mStatus = mTextureBuffer->Status()))
        { print_error("Failed to buffer texture#{}", mUID[]); }
}

void Texture::SetVariables(Farg<ThingData> data)
{
    Resource::SetVariables(data);

    data.GetVariable(mFormat.type, "Type");
    data.GetVariable(mFormat.data_format, "Format");
    data.GetVariable(mFormat.width, "Width");
    data.GetVariable(mFormat.height, "Height");
    data.GetVariable(mFormat.depth, "Depth");
    data.GetVariable(mFormat.array_layers, "ArrayLayers");
    data.GetVariable(mFormat.mipmaps, "MipMaps");
    data.GetVariable(mSampler.min_filter, "SamplerMinFilter", "MinFilter", "Min");
    data.GetVariable(mSampler.mip_filter_min, "SamplerMipFilterMin", "MipFilterMin", "MipMin");
    data.GetVariable(mSampler.mag_filter, "SamplerMagFilter", "MagFilter", "Mag");
    data.GetVariable(mSampler.mip_filter_mag, "SamplerMipFilterMag", "MipFilterMag", "MipMag");
    data.GetVariable(mSampler.repeat_u, "SamplerRepeatU", "SamplerRepeatX", "RepeatU", "RepeatX");
    data.GetVariable(mSampler.repeat_v, "SamplerRepeatV", "SamplerRepeatY", "RepeatV", "RepeatY");
    data.GetVariable(mSampler.repeat_w, "SamplerRepeatW", "SamplerRepeatZ", "RepeatW", "RepeatZ");
    data.GetVariable(mSampler.use_anisotropy, "UseAnisotropy", "AnisotropyEnabled");
    data.GetVariable(mSampler.anisotropy_max, "AnisotropyMax", "Anisotropy");
    // data.GetVariable(mBoundToFramebuffer, "Bound to Framebuffer");
}

Shared<ThingData> Texture::GetVariables() const
{
    Shared<ThingData> data{Resource::GetVariables()};
    data->AddVariable(mFormat.type, "Type");
    data->AddVariable(mFormat.data_format, "Format");
    data->AddVariable(mFormat.width, "Width");
    data->AddVariable(mFormat.height, "Height");
    data->AddVariable(mFormat.depth, "Depth");
    data->AddVariable(mFormat.array_layers, "ArrayLayers");
    data->AddVariable(mFormat.mipmaps, "MipMaps");
    data->AddVariable(mSampler.min_filter, "SamplerMinFilter");
    data->AddVariable(mSampler.mip_filter_min, "SamplerMipFilterMin");
    data->AddVariable(mSampler.mag_filter, "SamplerMagFilter");
    data->AddVariable(mSampler.mip_filter_mag, "SamplerMipFilterMag");
    data->AddVariable(mSampler.repeat_u, "SamplerRepeatU");
    data->AddVariable(mSampler.repeat_v, "SamplerRepeatV");
    data->AddVariable(mSampler.repeat_w, "SamplerRepeatW");
    data->AddVariable(mSampler.use_anisotropy, "UseAnisotropy");
    data->AddVariable(mSampler.anisotropy_max, "AnisotropyMax");
    // data->AddVariable(mBoundToFramebuffer, "Bound to Framebuffer");
    return data;
}

Shared<TextureBuffer> Texture::GetBuffer() const
{ return mTextureBuffer; }

Error Texture::SetBuffer(Shared<TextureBuffer> inBuffer)
{
    mStatus = inBuffer->Status();
    if(!print_error_enum(mStatus))
        { print_warning("Failed to replace Texture#{}'s TextureBuffer", mUID[]); }
    else
        { mTextureBuffer = inBuffer; }
    return mStatus;
}
