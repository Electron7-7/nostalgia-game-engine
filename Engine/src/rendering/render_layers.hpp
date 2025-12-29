#ifndef RENDER_LAYERS_H
#define RENDER_LAYERS_H

#include <format>

inline constinit const uint MaxRenderLayers{31}; // This is also the max that an `int` can be bitshift to the left

struct RenderLayers
{
public:
    static constexpr uint min{1};
    static constexpr uint max{MaxRenderLayers};
    static constexpr int all_enabled  {0b1111111111111111111111111111111};
    static constexpr int all_disabled {0b0000000000000000000000000000000}; // hopefully u get the point

    using StatusArray = std::array<bool, max>;

    RenderLayers(bool inAllLayersEnabled = true) noexcept;

    int get() const noexcept;
    void set(int inLayers) noexcept;
    bool contains(int inLayers) const noexcept;
    bool contains(RenderLayers inLayers) const noexcept;
    bool status(ushort inLayerIndex) const noexcept;
    StatusArray status() const noexcept;
    void enable(ushort inLayerIndex) noexcept;
    void disable(ushort inLayerIndex) noexcept;
    bool toggle(ushort inLayerIndex) noexcept;

    std::string log(bool noLayersStatus = false) const noexcept;

private:
    int layers_{0};
};

template<>
    struct std::formatter<RenderLayers> : std::formatter<std::string>
    {
        auto format(const RenderLayers& inT, std::format_context& ctx) const
            { return std::formatter<std::string>::format(inT.log(), ctx); }
    };

#endif // RENDER_LAYERS_H
