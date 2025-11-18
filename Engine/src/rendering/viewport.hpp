#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "math/containers.hpp"

struct Viewport
{
    Viewport(const scale_t& = {1, 1}, const position_t& = {0, 0});

    position_t position{};
    scale_t scale{};

    bool operator==(const Viewport&) const;
    bool operator!=(const Viewport&) const;
};

#endif // VIEWPORT_H
