#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "math/containers.hpp"
#include "core/id.hpp"

class Environment
{
public:
    enum BackgroundType : int
    {
        BG_CLEAR_COLOR,
        BG_CUSTOM_COLOR,
        BG_SKYBOX,
    };

    BackgroundType mType{BG_CLEAR_COLOR};
    ID mSkyboxTextureID{};
    ColorRGB mCustomColor{0.0};
    double   mCustomColorAlpha{0.0};
};

#endif // ENVIRONMENT_H
