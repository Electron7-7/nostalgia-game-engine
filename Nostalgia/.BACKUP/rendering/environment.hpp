#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

class Environment
{
public:
    enum BackgroundType : int
    {
        BG_CLEAR_COLOR,
        BG_CUSTOM_COLOR,
        BG_SKYBOX,
    };

    ColorRGBA<float> get_custom_color() const
    { return {mCustomColor[0], mCustomColor[1], mCustomColor[2], mCustomColorAlpha}; }

    BackgroundType mType{BG_CLEAR_COLOR};
    ID mSkyboxTextureID{};
    ColorRGB<float> mCustomColor{0.0};
    double   mCustomColorAlpha{0.0};
};

#endif // ENVIRONMENT_H
