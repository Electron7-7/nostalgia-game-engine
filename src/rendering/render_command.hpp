#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "render_context.hpp"
#include <vector>

struct RenderCommand
{
public:
    RenderCommand(RenderContext _RenderContext): render_context(_RenderContext) {}
    RenderCommand(): RenderCommand(RenderContext()) {}

    inline const RenderContext& GetRenderContext() const { return render_context; }

private:
    RenderContext render_context;
};

inline std::vector<RenderCommand> global_RenderCommandQueue = {};

#endif // RENDER_COMMAND_H