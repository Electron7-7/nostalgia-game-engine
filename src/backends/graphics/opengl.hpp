#ifndef OPENGL_BACKEND_H
#define OPENGL_BACKEND_H

#include "backends/backend.hpp"
#include "rendering/shader_interfaces/gl_shader.hpp"
#include <map>

class OpenGL_Backend : public GraphicsBackend
{
public:
    virtual ~OpenGL_Backend() = default;

    virtual bool Init();
    virtual void Shutdown();
    virtual const ShaderInterface* GetShader(unsigned int ShaderSelection) const;
    virtual bool BindShader(unsigned int ShaderLabel);
    virtual bool BuildShader(unsigned int ShaderLabel, const std::string& VertexShaderCode, const std::string& FragmentShaderCode);
    virtual bool RebuildShader(unsigned int ShaderLabel, const std::string& VertexShaderCode, const std::string& FragmentShaderCode);

    // PROTOTYPE FUNCTIONS
    virtual void prototype_ClearBuffer(glm::vec4 ClearColor);

private:
    std::map<unsigned int, GLShader> shaders;
    unsigned int currently_bound_shader = Shaders::SAFETY;
};

extern OpenGL_Backend singleton_OpenGL_Backend;

#endif // OPENGL_BACKEND_H
