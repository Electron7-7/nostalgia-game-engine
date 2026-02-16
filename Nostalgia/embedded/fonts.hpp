#ifndef FONTS_H
#define FONTS_H

#pragma clang diagostic push
#pragma clang diagnostic ignored "-Wc23-extensions"

namespace Fonts
{
    constexpr unsigned char Audiowide[]{
        #embed "fonts/Audiowide-Regular.ttf"
    };

    constexpr unsigned char DejaVuSans[]{
        #embed "fonts/DejaVuSansMono.ttf"
    };

    constexpr unsigned char Verdana[]{
        #embed "fonts/Verdana.ttf"
    };
}

#pragma clang diagostic pop
#endif // FONTS_H
