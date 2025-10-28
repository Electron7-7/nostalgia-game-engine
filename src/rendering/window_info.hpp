#ifndef WINDOW_INFO_H
#define WINDOW_INFO_H

#include <format>

struct ImVec2; // Forward declaration

struct scale_t
{
    constexpr scale_t() = default;
    constexpr scale_t(int Width, int Height): width{Width}, height{Height} {}
    scale_t(const ImVec2&);

    int width{0};
    int height{0};
    constexpr float AspectRatio() const { return static_cast<float>(width) / height; }

    explicit operator ImVec2() const;

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
    position_t(const ImVec2&);
    int x{0};
    int y{0};
    Origin origin{TopLeft};

    explicit operator ImVec2() const;

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
    position_t framebuffer_position{};

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

template<>
struct std::formatter<scale_t> : public std::formatter<int[2]>
{
    auto format(const scale_t& scale, std::format_context& ctx) const
    { return std::formatter<int[2]>::format({scale.width, scale.height}, ctx); }
};

template<>
struct std::formatter<position_t> : public std::formatter<int[2]>
{
    auto format(const position_t& position, std::format_context& ctx) const
    { return std::formatter<int[2]>::format({position.x, position.y}, ctx); }
};

#endif // WINDOW_INFO_H
