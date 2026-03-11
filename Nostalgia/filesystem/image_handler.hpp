#ifndef IMAGE_HANDLER_H
#define IMAGE_HANDLER_H

#include <Nostalgia/fwd/rendering.hpp>

namespace ImageHandler
{
    uchar* Load(Shared<FileData> inImageFile, TextureFormat& ioFormat, Error& outError);
    void Free(uchar* ioData);
}

#endif // IMAGE_HANDLER_H
