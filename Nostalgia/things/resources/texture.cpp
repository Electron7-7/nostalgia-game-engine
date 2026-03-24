#include "./texture.hpp"
#include "rendering/texture_buffer.hpp"
#include "filesystem/image_handler.hpp"

using namespace TheatreFile;

void Texture::Ready()
{ Import(); }

void Texture::SetVariables(Farg<ThingData> data)
{
    // Bypass `Resource::SetVariables`
    Thing::SetVariables(data);
    if(std::string path{}; data.get_variable(path, "Image", "File", "Data", "Path") == OK)
        { mStatus = try_LoadFileDataFromVariable(path, m_pFileData); }

    data.get_variable(mFormat.type, "Type");
    data.get_variable(mFormat.data_format, "Format");
    data.get_variable(mFormat.width, "Width");
    data.get_variable(mFormat.height, "Height");
    data.get_variable(mFormat.depth, "Depth");
    data.get_variable(mFormat.array_layers, "ArrayLayers");
    data.get_variable(mFormat.mipmaps, "MipMaps");
    data.get_variable(mSampler.min_filter, "SamplerMinFilter", "MinFilter", "Min");
    data.get_variable(mSampler.mip_filter_min, "SamplerMipFilterMin", "MipFilterMin", "MipMin");
    data.get_variable(mSampler.mag_filter, "SamplerMagFilter", "MagFilter", "Mag");
    data.get_variable(mSampler.repeat_u, "SamplerRepeatU", "SamplerRepeatX", "RepeatU", "RepeatX");
    data.get_variable(mSampler.repeat_v, "SamplerRepeatV", "SamplerRepeatY", "RepeatV", "RepeatY");
    data.get_variable(mSampler.repeat_w, "SamplerRepeatW", "SamplerRepeatZ", "RepeatW", "RepeatZ");
    data.get_variable(mSampler.use_anisotropy, "UseAnisotropy", "AnisotropyEnabled");
    data.get_variable(mSampler.anisotropy_max, "AnisotropyMax", "Anisotropy");
    // data.get_variable(mBoundToFramebuffer, "Bound to Framebuffer");

    Import();
}

Shared<ThingData> Texture::GetVariables() const
{
    // Bypass `Resource::GetVariables`
    Shared<ThingData> data{Thing::GetVariables()};
    if(m_pFileData->HasPath())
        { data->set_variable(m_pFileData->Path(), "Image"); }
    data->set_variable(mFormat.type, "Type");
    data->set_variable(mFormat.data_format, "Format");
    data->set_variable(mFormat.width, "Width");
    data->set_variable(mFormat.height, "Height");
    data->set_variable(mFormat.depth, "Depth");
    data->set_variable(mFormat.array_layers, "ArrayLayers");
    data->set_variable(mFormat.mipmaps, "MipMaps");
    data->set_variable(mSampler.min_filter, "SamplerMinFilter");
    data->set_variable(mSampler.mip_filter_min, "SamplerMipFilterMin");
    data->set_variable(mSampler.mag_filter, "SamplerMagFilter");
    data->set_variable(mSampler.repeat_u, "SamplerRepeatU");
    data->set_variable(mSampler.repeat_v, "SamplerRepeatV");
    data->set_variable(mSampler.repeat_w, "SamplerRepeatW");
    data->set_variable(mSampler.use_anisotropy, "UseAnisotropy");
    data->set_variable(mSampler.anisotropy_max, "AnisotropyMax");
    // data->set_variable(mBoundToFramebuffer, "Bound to Framebuffer");
    return data;
}

Error Texture::Import()
{
    mStatus = FAILED;
    mTextureBuffer = TextureBuffer::Create();
    VariableRegistry::try_GetResourceData(mUID(), m_pFileData);
    if(not m_pFileData)
    {
        print_error("No image data found/loaded");
        return mStatus;
    }

    auto image_data{ImageHandler::Load(m_pFileData, mFormat, mStatus)};

    if(print_error_enum(mStatus))
    {
        print_error("Failed to load image data for Texture ['{}', {}]",
            mName,
            mUID());
    }
    mStatus = mTextureBuffer->Load(image_data, mFormat);

    ImageHandler::Free(image_data);

    if(!print_error_enum(mStatus))
        { print_error("Failed to create Texture ['{}', {}]", mName, mUID()); }
    else
    {
        mTextureBuffer->GenerateMipMaps();
        mTextureBuffer->SetSamplerState(mSampler);
    }
    return mStatus;
}

Farg<TextureFormat> Texture::Format() const
{ return mFormat; }

void Texture::SetFormat(Farg<TextureFormat> inFormat)
{ mFormat = inFormat; }

Farg<SamplerState> Texture::Sampler() const
{ return mSampler; }

void Texture::SetSampler(Farg<SamplerState> inSampler)
{ mSampler = inSampler; }

Shared<TextureBuffer> Texture::GetBuffer() const
{ return mTextureBuffer; }

Error Texture::SetBuffer(Shared<TextureBuffer> inBuffer)
{
    mStatus = inBuffer->Status();
    if(!print_error_enum(mStatus))
    {
        print_warning("Failed to replace the TextureBuffer in Texture ['{}', {}]",
            mName,
            mUID());
    }
    else
        { mTextureBuffer = inBuffer; }
    return mStatus;
}
