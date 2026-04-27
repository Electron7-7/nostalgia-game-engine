#ifndef GL_GRAPHICS_CONTEXT_H
#define GL_GRAPHICS_CONTEXT_H

#include "rendering/graphics_context.hpp"

struct GLFWwindow;

class OpenGLContext : public IGraphicsContext
{
public:
    OpenGLContext(GLFWwindow* inNativeWindow);

    virtual Error Init() override;

private:
    GLFWwindow* mWindow{nullptr};
};

#endif // GL_GRAPHICS_CONTEXT_H
