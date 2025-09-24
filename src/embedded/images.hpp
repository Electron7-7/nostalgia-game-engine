#ifndef IMAGES_H
#define IMAGES_H

#include "things/id.hpp"

namespace Image
{
    constexpr unsigned char COMP04_5[] =
    {
        #embed "images/COMP04_5.png"
    };
    constexpr unsigned char Missing[] =
    {
        #embed "images/Missing.jpg"
    };
    constexpr unsigned char LolBit[] =
    {
        #embed "images/lolbit.png"
    };
    constexpr unsigned char LightDebugging[] =
    {
        #embed "images/LIGHT_DEBUGGING.jpg"
    };

    namespace Name
    {
        constexpr const char* Missing        = "MissingTexture";
        constexpr const char* COMP04_5       = "DoomTexture";
        constexpr const char* LolBit         = "LolBitTexture";
        constexpr const char* LightDebugging = "LightTexture";
    }

    namespace ID
    {
        constexpr id_t Missing        = 0;
        constexpr id_t LightDebugging = 1;
        constexpr id_t COMP04_5       = 2;
        constexpr id_t LolBit         = 3;
    }
}

#endif // IMAGES_H
