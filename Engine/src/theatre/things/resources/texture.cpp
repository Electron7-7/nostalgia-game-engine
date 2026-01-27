#include "texture.hpp"
#include "rendering/texture_buffer.hpp"
#include "theatre/variable_registry.hpp"

using namespace TheatreFile;

void Texture::Ready()
{
    VariableRegistry::try_GetResourceData(mUID, m_pImages[0]);
    if(UID::GetReservedType(mUID[]) == UID::ReservedType::Cubemap)
    {
        // Start at `1`, because the previous `try_GetResourceData` would've started us off
        for(uint i{1}; i < 6; ++i)
            {VariableRegistry::try_GetResourceData(mUID[] + i, m_pImages[i]); }
    }
    if(m_pImages.empty())
        { mTextureBuffer = TextureBuffer::Create(mFormat, mSampler, m_pFileData); }
    else
        { mTextureBuffer = TextureBuffer::Create(mFormat, mSampler, m_pImages); }
    if(!print_error_enum(mStatus = mTextureBuffer->Status()))
        { print_error("Failed to buffer texture#{}", mUID[]); }
}

void Texture::SetVariables(Farg<ThingData> data)
{
    Thing::SetVariables(data);
    std::string paths[6]{"","","","","",""};
    for(uint i{0}; i < 6; ++i)
    {
        std::string number{(i == 0) ? "" : std::to_string(i+1)};
        if(data.get_variable(paths[i], "Image" + number, "File" + number, "Data" + number, "FilePath" + number) == OK)
            { m_pImages[i] = MakeShared<FileData>(paths[i], FileType::Unknown); }
    }

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
    data.get_variable(mSampler.mip_filter_mag, "SamplerMipFilterMag", "MipFilterMag", "MipMag");
    data.get_variable(mSampler.repeat_u, "SamplerRepeatU", "SamplerRepeatX", "RepeatU", "RepeatX");
    data.get_variable(mSampler.repeat_v, "SamplerRepeatV", "SamplerRepeatY", "RepeatV", "RepeatY");
    data.get_variable(mSampler.repeat_w, "SamplerRepeatW", "SamplerRepeatZ", "RepeatW", "RepeatZ");
    data.get_variable(mSampler.use_anisotropy, "UseAnisotropy", "AnisotropyEnabled");
    data.get_variable(mSampler.anisotropy_max, "AnisotropyMax", "Anisotropy");
    // data.get_variable(mBoundToFramebuffer, "Bound to Framebuffer");
}

Shared<ThingData> Texture::GetVariables() const
{
    Shared<ThingData> data{Thing::GetVariables()};
    uint i{0};
    for(FAUTO image : m_pImages)
    {
        if(!image) { continue; }
        std::string number{(i == 0) ? "" : std::to_string(i+1)};
        if(image->HasPath())
            { data->set_variable(image->Path(), "Image" + number); }
    }
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
    data->set_variable(mSampler.mip_filter_mag, "SamplerMipFilterMag");
    data->set_variable(mSampler.repeat_u, "SamplerRepeatU");
    data->set_variable(mSampler.repeat_v, "SamplerRepeatV");
    data->set_variable(mSampler.repeat_w, "SamplerRepeatW");
    data->set_variable(mSampler.use_anisotropy, "UseAnisotropy");
    data->set_variable(mSampler.anisotropy_max, "AnisotropyMax");
    // data->set_variable(mBoundToFramebuffer, "Bound to Framebuffer");
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
