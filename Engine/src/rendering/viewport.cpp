#include "viewport.hpp"

Viewport::Viewport(const Scale2D& _scale, const Position2D& _position)
:   position{_position}, scale{_scale} {}

bool Viewport::operator==(const Viewport& other) const
{ return position == other.position && scale == other.scale; }

bool Viewport::operator!=(const Viewport& other) const
{ return !(*this == other); }
