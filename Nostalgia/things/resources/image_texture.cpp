#include "./image_texture.hpp"
#include "things/thing_factory.hpp"
#include "theatre/thing_data.hpp"
#include "theatre/theatre.hpp"

using namespace TheatreFile;

Shared<ImageTexture> ImageTexture::CreateFromImage(Farg<Shared<Image>> inImage)
{
    auto output{DCast<ImageTexture>(ThingFactory::MakeResource(ThingType::ImageTexture, "Untitled_ImageTexture"))};
    output->SetImage(inImage);
    return output;
}

void ImageTexture::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);
    mFormat.type = TEXTURE_TYPE_2D;
    if(data.get_variable(mInitialImageID, "Image", "Data") == OK)
        { SetImage(Theatre::Current()->GetResource<Image>(mInitialImageID)); }
    else if(std::string filepath; data.get_variable(filepath, "Image", "Data") == OK)
    {
        auto image{Image::CreateEmpty(0, 0)};
        image->LoadFile(filepath);
        SetImage(image);
    }
}

Shared<ThingData> ImageTexture::GetVariables() const
{
    auto data{Super::GetVariables()};
    data->set_variable(mInitialImageID, "Image");
    return data;
}

void ImageTexture::SetImage(Shared<Image> inImage)
{
    if(not mTextureBuffer)
        { mTextureBuffer = TextureBuffer::Create(); }

    if(not inImage or not inImage->raw_data())
    {
        print_error("No image data found/loaded");
        return;
    }

    mFormat = {inImage->Width(), inImage->Height(), inImage->Format()};

    if(not print_error_enum(mTextureBuffer->Load(inImage->raw_data(), mFormat)))
    {
        print_error("Failed to create Texture ['{}', {}]", name(), uid()());
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
            if(mSampler.mip_filter_min == SAMPLER_FILTER_NONE)
                { mSampler.mip_filter_min = SAMPLER_FILTER_LINEAR; }
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
