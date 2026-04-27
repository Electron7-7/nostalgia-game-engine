#include "graphics_context.hpp"
#include "renderer_api.hpp"
// Implementations
#include "backends/opengl/gl_graphics_context.hpp"

Unique<IGraphicsContext> IGraphicsContext::CreateContext(void* inNativeWindowPointer)
{
    switch(RendererAPI::CurrentAPI())
    {
    default:
    case RendererAPI::NONE:
        print_debug("Using the default graphics context (OpenGLContext)");
        [[fallthrough]];
    case RendererAPI::OPENGL:
        return MakeUnique<OpenGLContext>(static_cast<GLFWwindow*>(inNativeWindowPointer));
    }
}
