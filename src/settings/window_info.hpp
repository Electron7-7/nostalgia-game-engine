#ifndef SETTINGS_RETURN_TYPES_H
#define SETTINGS_RETURN_TYPES_H

#include <string>

struct scale_t
{
    constexpr scale_t() = default;
    constexpr scale_t(int Width, int Height): width{Width}, height{Height} {}
    int width{0};
    int height{0};
    constexpr float AspectRatio() const { return static_cast<float>(width) / height; }

    constexpr bool operator==(const scale_t& other) const
    { return width == other.width && height == other.height; }
    constexpr bool operator!=(const scale_t& other) const
    { return !(*this == other); }
};

struct position_t
{
    enum Origin
    {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };
    constexpr position_t() = default;
    constexpr position_t(int X, int Y, Origin Origin = TopLeft): x{X}, y{Y}, origin{Origin} {}
    int x{0};
    int y{0};
    Origin origin{TopLeft};

    constexpr bool operator==(const position_t& other) const
    { return x == other.x && y == other.y; }
    constexpr bool operator!=(const position_t& other) const
    { return !(*this == other); }
};

struct WindowInfo
{
    std::string name{""};
    position_t position{};
    scale_t size{};
    scale_t framebuffer_size{};

    constexpr bool operator==(const WindowInfo& other) const
    {
        return name == other.name &&
               position == other.position &&
               size == other.size &&
               framebuffer_size == other.framebuffer_size;
    }
    constexpr bool operator!=(const WindowInfo& other) const
    { return !(*this == other); }
};

#endif // SETTINGS_RETURN_TYPES_H
