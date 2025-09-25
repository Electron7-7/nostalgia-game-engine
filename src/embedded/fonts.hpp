#ifndef FONTS_H
#define FONTS_H

namespace Font
{

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

}

#endif // FONTS_H
