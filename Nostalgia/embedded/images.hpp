#ifndef IMAGES_H
#define IMAGES_H

#pragma clang diagostic push
#pragma clang diagnostic ignored "-Wc23-extensions"

namespace Images
{
    constexpr unsigned char Missing[]{
        #embed "images/Missing.jpg"
    };
    constexpr unsigned char COMP04_5[]{
        #embed "images/COMP04_5.png"
    };
    constexpr unsigned char LolBit[]{
        #embed "images/lolbit.png"
    };
    constexpr unsigned char LightDebug[]{
        #embed "images/LIGHT_DEBUGGING.jpg"
    };

    // Default Skybox
    constexpr unsigned char SkyboxXn[]{
        #embed "images/SHIT_SKYBOX_XNEG.png"
    };
    constexpr unsigned char SkyboxXp[]{
        #embed "images/SHIT_SKYBOX_XPOS.png"
    };
    constexpr unsigned char SkyboxYn[]{
        #embed "images/SHIT_SKYBOX_YNEG.png"
    };
    constexpr unsigned char SkyboxYp[]{
        #embed "images/SHIT_SKYBOX_YPOS.png"
    };
    constexpr unsigned char SkyboxZn[]{
        #embed "images/SHIT_SKYBOX_ZNEG.png"
    };
    constexpr unsigned char SkyboxZp[]{
        #embed "images/SHIT_SKYBOX_ZPOS.png"
    };
}

#pragma clang diagostic pop
#endif // IMAGES_H
