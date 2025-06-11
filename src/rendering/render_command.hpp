#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "render_context.hpp"

// FIXME: RenderCommand & RenderContext might be overkill and if RenderCommand doesn't really do anything but hold a RenderContext, I'd like to just replace RenderContext with RenderCommand
struct RenderCommand
{
public:
    RenderCommand(RenderContext _RenderContext): render_context(_RenderContext) {}
    RenderCommand(): RenderCommand(RenderContext()) {}

    const RenderContext& GetRenderContext() const { return render_context; }

private:
    RenderContext render_context;
};

#endif // RENDER_COMMAND_H
