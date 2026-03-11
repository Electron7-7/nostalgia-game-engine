#include "./image_handler.hpp"
#include "rendering/texture_buffer.hpp"
#define STB_IMAGE_IMPLEMENTATION
#ifdef NOSTALGIA_DEBUGGING
#   define STBI_FAILURE_USERMSG //generate user friendly error messages
#endif // NOSTALGIA_DEBUGGING
#include "stb_image/stb_image.h"

static int s_DataFormatToSTBI(DataFormat inFormat)
{
    switch(inFormat)
    {
    case DATA_FORMAT_RGB:
    case DATA_FORMAT_SRGB:
        return STBI_rgb;
    case DATA_FORMAT_RGBA:
    case DATA_FORMAT_RGBA8:
    case DATA_FORMAT_SRGB_ALPHA:
        return STBI_rgb_alpha;
    default:
        return STBI_default;
    }
}

uchar* ImageHandler::Load(Shared<FileData> inImage, TextureFormat& ioFormat, Error& outError)
{
    if(not inImage)
    {
        outError = ERR_NULLPTR;
        return nullptr;
    }
    else if(not print_error_enum(inImage->Status()))
    {
        outError = inImage->Status();
        return nullptr;
    }

    stbi_set_flip_vertically_on_load(true);

    auto ret_dat{stbi_load_from_memory(inImage->Data(),
        inImage->Size(),
        &ioFormat.width,
        &ioFormat.height,
        &ioFormat.channels,
        s_DataFormatToSTBI(ioFormat.data_format))};

    if(!ret_dat)
    {
        Free(ret_dat);
        print_error("STBI failed with reason: {}", stbi_failure_reason());
        outError = ERR_DATA_LOAD;
        return nullptr;
    }
    outError = OK;
    return ret_dat;
}

void ImageHandler::Free(uchar* ioData)
{ stbi_image_free(ioData); }
