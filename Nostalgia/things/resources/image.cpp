#include "./image.hpp"
#include "things/thing_factory.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG // generate user friendly error messages
#include "stb_image/stb_image.h"

static int sDataFormatToSTBI(DataFormat inFormat)
{
    switch(inFormat)
    {
        // return STBI_grey_alpha; // Unused
    case DATA_FORMAT_RED:
        return STBI_grey;
    case DATA_FORMAT_RGB:
    case DATA_FORMAT_SRGB:
        return STBI_rgb;
    case DATA_FORMAT_RGBA:
    case DATA_FORMAT_RGBA8:
    case DATA_FORMAT_SRGB_ALPHA:
        return STBI_rgb_alpha;
    default:
        print_warning("Invalid DataFormat encountered! Returning `STBI_default`");
        [[fallthrough]];
    case DATA_FORMAT_NONE:
        return STBI_default;
    }
}

Shared<Image> Image::CreateEmpty(int inWidth, int inHeight, bool inUseMipmaps, DataFormat inFormat)
{
    auto output{DCast<Image>(ThingFactory::MakeResource(ThingType::Image, "Untitled_Image"))};
    output->mWidth = inWidth;
    output->mHeight = inHeight;
    output->mUseMipmaps = inUseMipmaps;
    output->mFormat = inFormat;
    return output;
}

Shared<Image> Image::CreateFromData(int inWidth, int inHeight, bool inUseMipmaps, DataFormat inFormat,
    uchar* inImageData, int inImageDataSize)
{
    auto image{CreateEmpty(inWidth, inHeight, inUseMipmaps, inFormat)};
    image->SetData(inWidth, inHeight, inUseMipmaps, inFormat, inImageData, inImageDataSize);
    return image;
}

Farg<Shared<FileData>> Image::Data() const
{ return mData; }

Shared<FileData> Image::Data()
{ return mData; }

bool Image::UseMipmaps() const
{ return mUseMipmaps; }

int Image::Width() const
{ return mWidth; }

int Image::Height() const
{ return mHeight; }

DataFormat Image::Format() const
{ return mFormat; }

int Image::STBI_NumberOf8BitComponents() const
{ return mChannels; }

void Image::SetVariables(Farg<TheatreFile::ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mWidth, "Width");
    data.get_variable(mHeight, "Height");
    data.get_variable(mFormat, "Format");
    data.get_variable(mUseMipmaps, "Mipmaps");
    data.get_variable(mData, "File", "FilePath", "Path", "Image");


}

Shared<TheatreFile::ThingData> Image::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mWidth, "Width");
    data->set_variable(mHeight, "Height");
    data->set_variable(mFormat, "Format");
    data->set_variable(mUseMipmaps, "Mipmaps");
    data->set_variable(mData, "FilePath");
    return data;
}

void Image::Shutdown()
{
    Super::Shutdown();
    stbi_image_free(mData->raw_data());
}

Error Image::Load(Sarg inPath)
{
    auto image_file{MakeShared<FileData>(inPath)};
    if(not image_file->status())
    {
        print_error("Failed to load image file '{}'", inPath);
        return image_file->status();
    }

    switch(image_file->file_type())
    {
    case FileType::Unknown:
        print_warning("Unknown file type encountered. Since I don't account for all image types, Image::mFormat will be set to DATA_FORMAT_SRGB_ALPHA and image loading will continue");
        [[fallthrough]];
    case FileType::image_PNG:
        mFormat = DATA_FORMAT_SRGB_ALPHA;
        break;
    case FileType::image_JPG:
        mFormat = DATA_FORMAT_SRGB;
        break;
    default:
        print_error("Invalid file type encountered");
        return ERR_INVALID_TYPE;
    }

    SetData(0, 0, mUseMipmaps, mFormat, image_file->raw_data(), image_file->size());
    return OK;
}

void Image::SetData(int inWidth, int inHeight, bool inUseMipmaps, DataFormat inFormat,
    uchar* inImageData, int inImageDataSize)
{
    if(not inImageData)
        { print_warning("Input image data was null"); return; }

    mWidth  = inWidth;
    mHeight = inHeight;
    mFormat = inFormat;
    mUseMipmaps = inUseMipmaps;

#pragma message("TODO: figure out when to actually set this to true and when to not")
    stbi_set_flip_vertically_on_load(true);

    auto temp_data = stbi_load_from_memory(inImageData,
        inImageDataSize,
        &mWidth,
        &mHeight,
        &mChannels,
        sDataFormatToSTBI(mFormat));

    if(not temp_data)
    {
        print_error("STBI failed with reason: {}", stbi_failure_reason());
        stbi_image_free(temp_data);
        return;
    }

    mData->LoadData(temp_data, mWidth * mHeight * mChannels);
}
