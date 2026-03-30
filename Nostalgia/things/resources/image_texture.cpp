#include "./image_texture.hpp"
#include "theatre/thing_data.hpp"
#include "theatre/theatre.hpp"

using namespace TheatreFile;

void ImageTexture::SetVariables(Farg<ThingData> data)
{
    Super::Super::SetVariables(data);

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

    if(data.get_variable(mInitialImageID, "Image", "Data") == OK)
        { SetImage(Theatre::Current()->GetResource<Image>(mInitialImageID)); }
}

Shared<ThingData> ImageTexture::GetVariables() const
{
    auto data{Super::Super::GetVariables()};
    data->set_variable(mInitialImageID, "Image");
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
    return data;
}

TextureFormat ImageTexture::Format() const
{ return mFormat; }

void ImageTexture::SetImage(Shared<Image> inImage)
{
    if(not mTextureBuffer)
        { mTextureBuffer = TextureBuffer::Create(); }

    if(not inImage or inImage->uid().invalid() or not inImage->raw_data())
    {
        print_error("No image data found/loaded");
        return;
    }

    mFormat = {inImage->Width(), inImage->Height(), inImage->Format()};

    if(not print_error_enum(mTextureBuffer->Load(inImage->raw_data(), mFormat)))
    {
        print_error("Failed to create Texture ['{}', {}]", mName, uid()());
        return;
    }
    else
    {
        if(not inImage->UseMipmaps())
        {
            mSampler.mip_filter_min = SAMPLER_FILTER_NONE;
            mTextureBuffer->SetSamplerState(mSampler);
        }
        else
        {
            mTextureBuffer->SetSamplerState(mSampler);
            mTextureBuffer->GenerateMipMaps();
        }
    }
}

void ImageTexture::UpdateImage(Shared<Image> inImage)
{
    if(mFormat.width != inImage->Width()
        or mFormat.height != inImage->Height()
        or mFormat.data_format != inImage->Format()
        or mFormat.channels != inImage->Channels())
    {
        print_error("Image parameters do not match ImageTexture's parameters");
        print_debug("Image Params        - Width: {}, Height: {}, Channels: {}, DataFormat: {}",
            inImage->Width(),inImage->Height(),inImage->Channels(),EnumPrettifier::Prettify(inImage->Format()));
        print_debug("ImageTexture Params - Width: {}, Height: {}, Channels: {}, DataFormat: {}",
            mFormat.width, mFormat.height, mFormat.channels, EnumPrettifier::Prettify(mFormat.data_format));
        return;
    }

    print_error_enum(mTextureBuffer->Load(inImage->raw_data(), mFormat));
}
