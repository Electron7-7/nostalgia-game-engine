#include "./image_texture.hpp"
#include "things/thing_factory.hpp"
#include "things/thing_data.hpp"
#include "theatre/theatre.hpp"

#define LOCK LockGuard<RMutex> _lock{mBufferMutex};

using namespace TheatreFile;

Shared<ImageTexture> ImageTexture::CreateFromImage(Farg<Shared<Image>> inImage, SamplerState* inSampler)
{
    auto output{DCast<ImageTexture>(ThingFactory::MakeThing(ThingType::ImageTexture, "Untitled_ImageTexture"))};
    output->SetImage(inImage, inSampler);
    return output;
}

void ImageTexture::InitVariables()
{
    Super::InitVariables();
}

void ImageTexture::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);
    data.get_variable(mInitialImageID, "Image", "Data");
    if(not mInitialImageID.invalid())
        { SetImage(Theatre::Current()->GetResource<Image>(mInitialImageID)); }
    else if(data.get_variable(mInitialImagePath, "Image", "Data") == OK)
        { SetImage(Image::CreateFromFile(mInitialImagePath)); }
}

Shared<ThingData> ImageTexture::GetVariables() const
{
    auto data{Super::GetVariables()};
    data->set_variable(mInitialImageID, "Image");
    data->set_variable(mInitialImagePath, "Image");
    return data;
}

void ImageTexture::SetImage(Shared<Image> inImage, SamplerState* inSampler)
{
    LOCK
    if(not inImage or not inImage->data())
    {
        print_error("No image data found/loaded");
        return;
    }

    SamplerState _sampler{};
    if(inSampler)
        { _sampler = *inSampler; }
    else
        { mBuffer->GetSamplerState(_sampler); }

    mBuffer = TextureBuffer::Create({TEXTURE_TYPE_2D, inImage->Width(), inImage->Height(), inImage->Format()});
    mBuffer->SetData({inImage->data()});
    mBuffer->SetSamplerState(_sampler);
}

void ImageTexture::UpdateImage(Shared<Image> inImage)
{
    LOCK
    auto _format{mBuffer->GetFormat()};
    if(_format.width != inImage->Width()
        or _format.height != inImage->Height()
        or _format.data_format != inImage->Format())
    {
        print_error("Image parameters do not match ImageTexture's parameters");
        print_debug("Image Params        - Width: {}, Height: {}, DataFormat: {}",
            inImage->Width(), inImage->Height(), EnumRegistry::GetEnumName(inImage->Format()));
        print_debug("ImageTexture Params - Width: {}, Height: {}, DataFormat: {}",
            _format.width, _format.height, EnumRegistry::GetEnumName(_format.data_format));
        return;
    }

    mInitialImageID = (mInitialImageID.invalid()) ? inImage->uid() : mInitialImageID;
    mBuffer->SetData({inImage->data(), 0, inImage->Width(), inImage->Height()});
}
