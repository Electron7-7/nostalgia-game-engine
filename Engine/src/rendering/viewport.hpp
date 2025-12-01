#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "math/containers.hpp"

struct Viewport
{
    Viewport(const scale_t& = {1, 1}, const Position& = {0, 0});

    Position position{};
    scale_t scale{};

    bool operator==(const Viewport&) const;
    bool operator!=(const Viewport&) const;
};

#endif // VIEWPORT_H
