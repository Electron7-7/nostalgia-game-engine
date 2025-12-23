#include "viewport.hpp"

Viewport::Viewport(Farg<Scale2D> inScale, Farg<Position2D> inPosition, RenderLayers inLayers) noexcept:
    position{inPosition},
    scale{inScale},
    layers{inLayers} {}

bool Viewport::operator==(const Viewport& other) const
{ return position == other.position && scale == other.scale; }

bool Viewport::operator!=(const Viewport& other) const
{ return !(*this == other); }
