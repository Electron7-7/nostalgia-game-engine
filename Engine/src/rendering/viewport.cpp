#include "viewport.hpp"

Viewport::Viewport(const scale_t& _scale, const Position& _position)
:   position{_position}, scale{_scale} {}

bool Viewport::operator==(const Viewport& other) const
{ return position == other.position && scale == other.scale; }

bool Viewport::operator!=(const Viewport& other) const
{ return !(*this == other); }
