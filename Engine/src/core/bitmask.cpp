#include <nostalgia_pch.hpp>

BitMask::BitMask(bool inState) noexcept:
    layers_{(inState) ? all_enabled : all_disabled} {}

BitMask::BitMask(const StatusArray& inStatusArray) noexcept:
    BitMask()
{ set(inStatusArray); }

int BitMask::get() const noexcept
{ return layers_; }

void BitMask::set(int inLayers) noexcept
{ layers_ = inLayers; }

void BitMask::set(const StatusArray& inArray) noexcept
{
    for(int i{0}; i < inArray.size(); ++i)
        { set_layer(i, inArray[i]); }
}

bool BitMask::contains(int inLayers) const noexcept
{ return layers_ & inLayers; }

bool BitMask::contains(BitMask inLayers) const noexcept
{ return layers_ & inLayers.get(); }

bool BitMask::status(ushort index) const noexcept
{
    int layer{1 << index};
    return (layers_ & layer) == layer;
}

BitMask::StatusArray BitMask::status() const noexcept
{
    StatusArray output{};
    for(int i{0}; i < max; ++i)
        { output[i] = status(i); }
    return output;
}

void BitMask::set_layer(ushort index, bool state) noexcept
{
    if(state) { enable(index); }
    else { disable(index); }
}

void BitMask::enable(ushort index) noexcept
{ layers_ |= (1 << index); }

void BitMask::disable(ushort index) noexcept
{ layers_ &= ~(1 << index); }

bool BitMask::toggle(ushort index) noexcept
{
    layers_ ^= (1 << index);
    return status(index);
}

std::string BitMask::log(bool noLayersStatus) const noexcept
{
    std::string output{std::format("(count: {})", max)};
    if(!noLayersStatus)
    {
        output += " [";
        for(int i{0}; i < max; ++i)
        {
            output += std::format("{}{}",
                status(i),
                (i < max - 1)
                    ? ", "
                    : "]");
        }
    }
    return output;
}
