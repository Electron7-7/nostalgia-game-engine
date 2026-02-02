#ifndef BIT_MASK_H
#define BIT_MASK_H


inline constinit const uint MaxBitMaskPlaces{30};

struct BitMask
{
public:
    static constexpr uint min{0};
    static constexpr uint max{MaxBitMaskPlaces};
    static constexpr int all_enabled  {0b111111111111111111111111111111};
    static constexpr int all_disabled {0b000000000000000000000000000000}; // hopefully u get the point
    using StatusArray = std::array<bool, max>;

    BitMask(bool inAllLayersEnabled = true) noexcept;
    BitMask(const StatusArray& inStatusArray) noexcept;

    int get() const noexcept;
    void set(int inBits) noexcept;
    void set(const StatusArray& inStatusArray) noexcept;
    bool contains(int inBits) const noexcept;
    bool contains(BitMask inLayers) const noexcept;
    bool status(ushort inLayerIndex) const noexcept;
    StatusArray status() const noexcept;
    void set_index(ushort inLayerIndex, bool inStatus) noexcept;
    void enable_index(ushort inLayerIndex) noexcept;
    void disable_index(ushort inLayerIndex) noexcept;
    bool toggle_index(ushort inLayerIndex) noexcept;
    void set(int inBits, bool inStatus) noexcept;
    void enable(int inBits) noexcept;
    void disable(int inBits) noexcept;
    void toggle(int inBits) noexcept;

    std::string log(bool noLayersStatus = false) const noexcept;

    constexpr bool operator==(const BitMask& inLayers) const noexcept
    { return layers_ == inLayers.layers_; }

private:
    int layers_{0};
};

template<>
    struct std::formatter<BitMask> : std::formatter<std::string>
    {
        auto format(const BitMask& inT, std::format_context& ctx) const
            { return std::formatter<std::string>::format(inT.log(), ctx); }
    };

#endif // BIT_MASK_H
