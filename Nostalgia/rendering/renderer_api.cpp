#include "renderer_api.hpp"

// Implementations
#include "backends/opengl/gl_renderer_api.hpp"

Unique<RendererAPI> RendererAPI::Activate()
{
#pragma message("TODO: When I implement different APIs, also implement a way to switch them")
    sAPI = GraphicsAPI::OpenGL;
    std::string warning_api_name{"GraphicsAPI::None"};
    switch(sAPI)
    {
    default:
        warning_api_name = "Invalid";
        [[fallthrough]];
    case GraphicsAPI::None:
        print_warning("Current API is {}. The default will be used (OpenGL).", warning_api_name);
        sAPI = GraphicsAPI::OpenGL;
        [[fallthrough]];
    case GraphicsAPI::OpenGL:
        return MakeUnique<OpenGLRendererAPI>();
        break;
    }
}
