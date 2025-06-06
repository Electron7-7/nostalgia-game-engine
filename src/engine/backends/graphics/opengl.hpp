#ifndef OPENGL_BACKEND_H
#define OPENGL_BACKEND_H

#include "engine/backends/backend.hpp"

class OpenGL_Backend : public GraphicsBackend
{
public:
    virtual ~OpenGL_Backend() = default;

    virtual bool Init();
    virtual void Shutdown();

    // PROTOTYPE FUNCTIONS
    virtual void prototype_ClearBuffer(glm::vec4 ClearColor);

private:
};

extern OpenGL_Backend singleton_OpenGL_Backend;

#endif // OPENGL_BACKEND_H
