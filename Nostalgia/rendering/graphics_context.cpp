#include "graphics_context.hpp"
#include "renderer_api.hpp"
// Implementations
#include "backends/opengl/gl_graphics_context.hpp"

Unique<GraphicsContext> GraphicsContext::CreateDummy()
{ return MakeUnique<DummyGraphicsContext>(); }

Unique<GraphicsContext> GraphicsContext::Create(void* inNativeWindowPointer)
{
    switch(RendererAPI::CurrentAPI())
    {
    default:
    case RendererAPI::NONE:
        print_error(RendererAPI::s_cAPIWarningMessage);
        return CreateDummy();
    case RendererAPI::OPENGL:
        return MakeUnique<OpenGLContext>(static_cast<GLFWwindow*>(inNativeWindowPointer));
    }
}
