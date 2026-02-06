#include "./texture.hpp"
#include "rendering/texture_buffer.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

using namespace TheatreFile;

void Texture::Ready()
{ Import(); }

void Texture::SetVariables(Farg<ThingData> data)
{
    Resource::SetVariables(data);
    if(m_pFileData->HasPath())
        { m_pImages[0] = m_pFileData; }
    else if(std::string path{}; data.get_variable(path, "Image") == OK)
        { m_pImages[0] = MakeShared<FileData>(path); }

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

    for(uint i{0}; i < 6; ++i)
    {
        if(std::string path{}; data.get_variable(path, "Image" + std::to_string(i)) == OK)
        {
            m_pImages[i] = MakeShared<FileData>();
            print_error_enum(m_pImages[i]->LoadFile(path));
        }
    }
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
    VariableRegistry::try_GetResourceData(mUID[], m_pImages[0]);

    for(int i{0}; i < 6; ++i)
    {
        if(UID::GetReservedType(mUID[]) == UID::ReservedType::Cubemap)
            { VariableRegistry::try_GetResourceData(mUID[] + i, m_pImages[i]); }

        if(auto data{m_pImages[i]})
        {
            stbi_set_flip_vertically_on_load(true);

            auto image_data{stbi_load_from_memory(data->Data(),
                data->Size(),
                &mFormat.width,
                &mFormat.height,
                &mFormat.channels,
                STBI_rgb)};

            if(!image_data)
            {
                print_error("STBI failed to load image data for Texture ['{}', {}]",
                    mName,
                    mUID[]);
                mStatus = ERR_DATA_LOAD;
            }
            else if(mTextureBuffer->Load(image_data, mFormat) == OK)
                { mStatus = OK; }
            stbi_image_free(image_data);
        }
    }

    if(!print_error_enum(mStatus))
        { print_error("Failed to create Texture ['{}', {}]", mName, mUID[]); }
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
            mUID[]);
    }
    else
        { mTextureBuffer = inBuffer; }
    return mStatus;
}
