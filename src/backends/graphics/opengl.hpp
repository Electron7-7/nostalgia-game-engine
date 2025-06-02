#ifndef OPENGL_BACKEND_H
#define OPENGL_BACKEND_H

#include <backends/backends.hpp>
#include <glad/glad.h>

class OpenGL_Backend : public GraphicsBackend
{
    virtual bool Init();
    virtual void Shutdown();

    // PROTOTYPE FUNCTIONS
    virtual void prototype_ClearBuffer(glm::vec4 ClearColor);
    virtual void prototype_RenderFrame();
};

extern OpenGL_Backend singleton_OpenGL_Backend;

#endif // OPENGL_BACKEND_H