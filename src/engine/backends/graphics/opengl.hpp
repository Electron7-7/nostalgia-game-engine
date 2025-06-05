#ifndef OPENGL_BACKEND_H
#define OPENGL_BACKEND_H

#include "engine/backends/backend.hpp"
// #include "common/opengl_includes.hpp"

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