#include "./image.hpp"
#include "things/thing_data.hpp"
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

Image::~Image() noexcept
{ free(); }

Shared<Image> Image::CreateEmpty(int inWidth, int inHeight, DataFormat inFormat)
{
    auto output{DCast<Image>(ThingFactory::MakeThing(ThingType::Image, "Untitled_Image"))};
    output->SetData(nullptr, 0, inFormat);
    output->mWidth  = inWidth;
    output->mHeight = inHeight;
    return output;
}

Shared<Image> Image::CreateFromData(const uchar* inImageData, int inImageDataSize, DataFormat inFormat)
{
    auto output{DCast<Image>(ThingFactory::MakeThing(ThingType::Image, "Untitled_Image"))};
    output->SetData(inImageData, inImageDataSize, inFormat);
    output->Import();
    return output;
}

void Image::GetInfo(Farg<Shared<FileData>> inFile, int* outWidth, int* outHeight, int* outChannels)
{ stbi_info_from_memory(inFile->raw_data(), inFile->size(), outWidth, outHeight, outChannels); }

void Image::SetVariables(Farg<TheatreFile::ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mWidth, "Width");
    data.get_variable(mHeight, "Height");
    data.get_variable(mFormat, "Format");
    data.get_variable(mUseMipmaps, "Mipmaps");
    if(data.get_variable(m_pFileData, "File", "FilePath", "Path", "Image") == OK)
        { Import(); }
}

Shared<TheatreFile::ThingData> Image::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mWidth, "Width");
    data->set_variable(mHeight, "Height");
    data->set_variable(mFormat, "Format");
    data->set_variable(mUseMipmaps, "Mipmaps");
    data->set_variable(m_pFileData, "FilePath");
    return data;
}

void Image::Shutdown()
{
    Super::Shutdown();
    free();
}

Error Image::Import()
{
    free();
    if(not m_pFileData)
        { return ERR_NULLPTR; }
    else if(not print_error_enum(m_pFileData->status()))
        { return ERR_INVALID; }

#pragma message("TODO: figure out when to actually set this to true and when to not")
    stbi_set_flip_vertically_on_load(true);

    m_pImage = stbi_load_from_memory(m_pFileData->raw_data(),
        m_pFileData->size(),
        &mWidth,
        &mHeight,
        &mChannels,
        sDataFormatToSTBI(mFormat));

    if(not m_pImage)
    {
        print_error("STBI failed with reason: {}", stbi_failure_reason());
        return ERR_DATA_LOAD;
    }

    mSize = mWidth * mHeight * mChannels;
    return OK;
}

Error Image::LoadFile(Sarg inPath)
{
    if(not m_pFileData->LoadFile(inPath))
    {
        print_error("Failed to load image file '{}'", inPath);
        return ERR_FILE_LOAD;
    }
    switch(m_pFileData->file_type())
    {
    case FileType::Unknown:
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
    return Import();
}

void Image::SetData(const uchar* inImageData, int inImageDataSize, DataFormat inFormat)
{
    mFormat = inFormat;

    if(not inImageData)
        { return; }

    GetInfo(m_pFileData = MakeShared<FileData>(inImageData, inImageDataSize),
        &mWidth, &mHeight, &mChannels);
}

void Image::free()
{
    if(m_pImage)
    {
        stbi_image_free(m_pImage);
        m_pImage = nullptr;
        mSize = 0;
    }
}

Farg<FileData> Image::file_data() const
{ return *m_pFileData; }

const uchar* Image::raw_data() const
{ return m_pImage; }

uchar* Image::raw_data()
{ return m_pImage; }

int Image::size() const
{ return mSize; }

Sarg Image::get_filepath() const
{ return m_pFileData->filepath(); }

bool Image::UseMipmaps() const
{ return mUseMipmaps; }

void Image::SetUseMipmaps(bool inUseMipmaps)
{ mUseMipmaps = inUseMipmaps; }

DataFormat Image::Format() const
{ return mFormat; }

void Image::SetFormat(DataFormat inFormat)
{ mFormat = inFormat; }

int Image::Width() const
{ return mWidth; }

int Image::Height() const
{ return mHeight; }

int Image::Channels() const
{ return mChannels; }
