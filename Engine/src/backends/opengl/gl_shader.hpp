#ifndef GL_SHADER_H
#define GL_SHADER_H

#include "rendering/shader.hpp"

class GLShader : public IShader
{
public:
    virtual ~GLShader() = default;

    bool CompileShader(FARG(std::string) VertexShaderCode, FARG(std::string) FragmentShaderCode);

    void SetUniform(FARG(std::string), FARG(int))       const final;
    void SetUniform(FARG(std::string), FARG(float))     const final;
    void SetUniform(FARG(std::string), FARG(glm::vec2)) const final;
    void SetUniform(FARG(std::string), FARG(glm::vec3)) const final;
    void SetUniform(FARG(std::string), FARG(glm::vec4)) const final;
    void SetUniform(FARG(std::string), FARG(glm::mat3)) const final;
    void SetUniform(FARG(std::string), FARG(glm::mat4)) const final;

private:
    bool GLShaderErrorHandler(FARG(ID), bool IsLinkingInsteadOfCompiling = false) const;
    bool GLShaderClear(FARG(ID));
};

#endif // GL_SHADER_H
