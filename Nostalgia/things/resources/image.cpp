#include "./image.hpp"
#include "things/thing_data.hpp"
#include "things/thing_factory.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG // generate user friendly error messages
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

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
    auto output{DCast<Image>(ThingFactory::MakeThing<Image>("Image"))};
    output->SetData(nullptr, 0, inWidth, inHeight, 1, true, inFormat);
    return output;
}

Shared<Image> Image::CreateFromFile(Farg<FileData> inFile, DataFormat inFormat)
{
    auto output{Image::CreateEmpty(0, 0)};
    print_error_enum(output->LoadFile(inFile, inFormat));
    return output;
}

Shared<Image> Image::CreateFromData(uchar* inData, int inSize, int inWidth, int inHeight, int inChannels,
    bool inUseMipmaps, DataFormat inFormat)
{
    auto output{Image::CreateEmpty(0, 0)};
    output->SetData(inData, inSize, inWidth, inHeight, inChannels, inUseMipmaps, inFormat);
    return output;
}

void Image::GetInfo(Farg<Shared<FileData>> inFile, int* outWidth, int* outHeight, int* outChannels)
{ stbi_info_from_memory(inFile->raw_data(), inFile->size(), outWidth, outHeight, outChannels); }

void Image::InitVariables()
{
    Super::InitVariables();
}

void Image::SetVariables(Farg<TheatreFile::ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mWidth, "Width");
    data.get_variable(mHeight, "Height");
    data.get_variable(mFormat, "Format");
    data.get_variable(mUseMipmaps, "Mipmaps");
    if(data.get_variable(mFilepath, "File", "FilePath", "Path", "Image") == OK)
        { LoadFile(mFilepath, mFormat); }
}

Shared<TheatreFile::ThingData> Image::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mWidth, "Width");
    data->set_variable(mHeight, "Height");
    data->set_variable(mFormat, "Format");
    data->set_variable(mUseMipmaps, "Mipmaps");
    data->set_variable(mFilepath, "FilePath",
        TheatreFile::VARIABLE_HINT_FILE, FileSystem::CreateFileExtensionFilter("Image Files", ".png,.jpg,.jpeg"));
    return data;
}

void Image::Shutdown()
{
    Super::Shutdown();
    free();
}

Error Image::LoadFile(Farg<FileData> inFile, DataFormat inFormat)
{
    free();

    mFormat = inFormat;

    if(not print_error_enum(inFile.status()))
        { return ERR_INVALID; }

    stbi_set_flip_vertically_on_load(true);

    m_pImage = stbi_load_from_memory(inFile.raw_data(),
        inFile.size(),
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
    mFilepath = inFile.filepath();
    mAllocatedWithSTB = true;
    return OK;
}

void Image::SetData(uchar* inData, int inSize, int inWidth, int inHeight, int inChannels,
    bool inUseMipmaps, DataFormat inFormat)
{
    free();
    m_pImage = new uchar[inSize];
    mSize = inSize;
    mWidth = inWidth;
    mHeight = inHeight;
    mChannels = inChannels;
    mUseMipmaps = inUseMipmaps;
    mFormat = inFormat;
    mFilepath.clear();

    for(int i{0}; i < inSize; ++i)
        { m_pImage[i] = inData[i]; }

    mAllocatedWithNew = true;
    mAllocatedWithSTB = false;
}

SafeReturn<std::string> Image::SaveJPG(Sarg inPath, FileSystem::OverwriteAction inAction, int inQuality)
{
    if(not m_pImage)
        { return ERR_NULLPTR; }
    auto _test_write{FileSystem::Lazy::Write(inPath, {}, inAction, true)};
    if(not _test_write.status())
        { return _test_write; }
    stbi_flip_vertically_on_write(true);
    if(not stbi_write_jpg(_test_write.data().data(), mWidth, mHeight, mChannels, m_pImage, inQuality))
    {
        print_error("STBI failed to write file with reason: {}", stbi_failure_reason());
        return ERR_FILE_WRITE;
    }
    return OK;
}

void Image::free()
{
    if(m_pImage)
    {
        if(mAllocatedWithSTB)
            { stbi_image_free(m_pImage); }
        else if(mAllocatedWithNew)
            { delete [] m_pImage; }
    }
    m_pImage = nullptr;
    mAllocatedWithSTB = false;
    mSize = 0;
}

const uchar* Image::data() const
{ return m_pImage; }

uchar* Image::data()
{ return m_pImage; }

int Image::size() const
{ return mSize; }

Sarg Image::filepath() const
{ return mFilepath; }

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
