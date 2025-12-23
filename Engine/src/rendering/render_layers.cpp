#include "render_layers.hpp"

RenderLayers::RenderLayers(bool inState) noexcept:
    layers_{(inState) ? all_enabled : all_disabled} {}

int RenderLayers::get() const noexcept
{ return layers_; }

void RenderLayers::set(int inLayers) noexcept
{ layers_ = inLayers; }

bool RenderLayers::contains(int inLayers) const noexcept
{ return layers_ & inLayers; }

bool RenderLayers::contains(RenderLayers inLayers) const noexcept
{ return layers_ & inLayers.get(); }

bool RenderLayers::status(ushort index) const noexcept
{
    int layer{1 << index};
    return (layers_ & layer) == layer;
}

void RenderLayers::enable(ushort index) noexcept
{ layers_ |= (1 << index); }

void RenderLayers::disable(ushort index) noexcept
{ layers_ |= ~(1 << index); }

bool RenderLayers::toggle(ushort index) noexcept
{
    layers_ ^= (1 << index);
    return status(index);
}

std::string RenderLayers::log(bool noLayersStatus) const noexcept
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

RenderLayers::StatusArray RenderLayers::status() const noexcept
{
    std::array<bool, max> output{};
    for(int i{0}; i < max; ++i)
        { output[i] = status(i); }
    return output;
}
