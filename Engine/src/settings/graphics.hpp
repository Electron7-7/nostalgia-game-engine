#ifndef GRAPHICS_SETTINGS_H
#define GRAPHICS_SETTINGS_H

namespace Settings
{
    namespace Graphics
    {
        enum class StretchMode   { Disabled, Viewport };
        enum class StretchAspect { Ignore, Keep, KeepWidth, KeepHeight, };

        inline bool GlobalWireframe{false};
        inline ColorRGBA ClearColor{0.29f, 0.34f, 0.26f, 1.0f};

        namespace Stretch
        {
            inline StretchMode Mode{StretchMode::Disabled};
            inline StretchAspect Aspect{StretchAspect::Ignore};
        }
    }
}

#endif // GRAPHICS_SETTINGS_H
