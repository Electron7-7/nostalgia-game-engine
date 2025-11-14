#ifndef CONTAINERS_H
#define CONTAINERS_H

#include <cstdint>

struct ImVec2;

struct scale_t
{
    constexpr scale_t() = default;
    constexpr scale_t(uint32_t Width, uint32_t Height): width{Width}, height{Height} {}
    scale_t(const ImVec2&);

    uint32_t width{0};
    uint32_t height{0};
    constexpr float AspectRatio() const { return static_cast<float>(width) / height; }

    explicit operator ImVec2() const;

    constexpr bool operator==(const scale_t& other) const
    { return width == other.width && height == other.height; }
};

struct position_t
{
    enum class Origin
    {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
    };
    constexpr position_t() = default;
    constexpr position_t(int X, int Y, Origin Origin = Origin::TopLeft): x{X}, y{Y}, origin{Origin} {}
    position_t(const ImVec2&);
    int x{0};
    int y{0};
    Origin origin{Origin::TopLeft};

    explicit operator ImVec2() const;

    constexpr bool operator==(const position_t& other) const
    { return x == other.x && y == other.y; }
};

typedef const scale_t& ScaleArg;
typedef const position_t& PositionArg;

#endif // CONTAINERS_H
