#ifndef OPENGL_BACKEND_H
#define OPENGL_BACKEND_H

#include "engine/backends/backend.hpp"
#include "engine/rendering/shader_interfaces/gl_shader.hpp"
#include <vector>

class OpenGL_Backend : public GraphicsBackend
{
public:
    virtual ~OpenGL_Backend() = default;

    virtual bool Init();
    virtual void Shutdown();
    virtual const ShaderInterface* GetShader(unsigned int ShaderSelection) const;

    // PROTOTYPE FUNCTIONS
    virtual void prototype_ClearBuffer(glm::vec4 ClearColor);

private:
    std::vector<GLShader> shaders;
};

extern OpenGL_Backend singleton_OpenGL_Backend;

#endif // OPENGL_BACKEND_H
