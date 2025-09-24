#ifndef FONTS_H
#define FONTS_H

namespace Font
{
    #include "things/id.hpp"

    constexpr unsigned char Audiowide[] =
    {
        #embed "fonts/Audiowide-Regular.ttf"
    };
    constexpr unsigned char DejaVuSansMono[] =
    {
        #embed "fonts/DejaVuSansMono.ttf"
    };
    constexpr unsigned char Verdana[] =
    {
        #embed "fonts/Verdana.ttf"
    };

    namespace Name
    {
        constexpr const char* Audiowide      = "Audiowide";
        constexpr const char* Verdana        = "Verdana";
        constexpr const char* DejaVuSansMono = "DejaVuSansMono";
    }

    namespace ID
    {
        constexpr id_t Verdana        = 0;
        constexpr id_t DejaVuSansMono = 1;
        constexpr id_t Audiowide      = 2;
    }
}

#endif // FONTS_H
