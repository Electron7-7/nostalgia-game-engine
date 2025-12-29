#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "rendering/render_layers.hpp"
#include "core/farg.hpp"
#include "math/containers.hpp"

struct Viewport
{
    struct IDs
    {
        static constexpr uint MainWindow{1};
    };

    Viewport(Farg<Scale2D> = {1, 1}, Farg<Position2D> = {0, 0}, RenderLayers = {}) noexcept;

    Position2D position{};
    Scale2D scale{};
    RenderLayers layers{};

    bool operator==(Farg<Viewport>) const;
    bool operator!=(Farg<Viewport>) const;
};

#endif // VIEWPORT_H
