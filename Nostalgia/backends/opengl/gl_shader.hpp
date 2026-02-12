#ifndef GL_SHADER_H
#define GL_SHADER_H

#include <Nostalgia/rendering/shader.hpp>

class GLShader : public Shader
{
public:
    virtual ~GLShader() = default;

    bool CompileShader(Farg<std::string>, Farg<std::string>) final;

    void Bind()   const final;
    void Unbind() const final;

    void SetUniform(Farg<std::string>, Farg<int>)       const final;
    void SetUniform(Farg<std::string>, Farg<float>)     const final;
    void SetUniform(Farg<std::string>, Farg<glm::vec2>) const final;
    void SetUniform(Farg<std::string>, Farg<glm::vec3>) const final;
    void SetUniform(Farg<std::string>, Farg<glm::vec4>) const final;
    void SetUniform(Farg<std::string>, Farg<glm::mat3>) const final;
    void SetUniform(Farg<std::string>, Farg<glm::mat4>) const final;

private:
    bool GLShaderErrorHandler(Farg<ID>, bool IsLinkingInsteadOfCompiling = false) const;
    bool GLShaderClear(Farg<ID>);
};

#endif // GL_SHADER_H
