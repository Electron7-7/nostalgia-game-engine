#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "math/containers.hpp"

struct Viewport
{
    Viewport(const Scale2D& = {1, 1}, const Position2D& = {0, 0});

    Position2D position{};
    Scale2D scale{};

    bool operator==(const Viewport&) const;
    bool operator!=(const Viewport&) const;
};

#endif // VIEWPORT_H
