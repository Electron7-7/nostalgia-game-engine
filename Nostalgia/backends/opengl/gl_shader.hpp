#ifndef GL_SHADER_H
#define GL_SHADER_H

#include "rendering/shader.hpp"

class GLShader : public Shader
{
public:
    virtual ~GLShader() = default;

    bool CompileShader(Sarg, Sarg) final;

    void Bind()   const final;
    void Unbind() const final;

    void SetUniform(Sarg, int)             const final;
    void SetUniform(Sarg, float)           const final;
    void SetUniform(Sarg, Farg<glm::vec2>) const final;
    void SetUniform(Sarg, Farg<glm::vec3>) const final;
    void SetUniform(Sarg, Farg<glm::vec4>) const final;
    void SetUniform(Sarg, Farg<glm::mat3>) const final;
    void SetUniform(Sarg, Farg<glm::mat4>) const final;

private:
    bool GLShaderErrorHandler(Farg<ID>, bool IsLinkingInsteadOfCompiling = false) const;
    bool GLShaderClear(Farg<ID>);
};

#endif // GL_SHADER_H
