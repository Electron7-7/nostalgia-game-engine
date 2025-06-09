#ifndef GL_SHADER_H
#define GL_SHADER_H

#include "shader_interface.hpp"

class GLShader : public ShaderInterface
{
public:
    virtual ~GLShader() = default;

    GLShader(const std::string& VertexShaderCode, const std::string& FragmentShaderCode);

    void Bind() const final;

    void SetUniform(const std::string& Name, int Value) const final;
    void SetUniform(const std::string& Name, float Value) const final;
    void SetUniform(const std::string& Name, glm::vec2 Value) const final;
    void SetUniform(const std::string& Name, glm::vec3 Value) const final;
    void SetUniform(const std::string& Name, glm::mat4 Value) const final;

private:
    void GLShaderErrorHandler(unsigned int ID, bool IsLinkingInsteadOfCompiling = false) const;
};

#endif // GL_SHADER_H
