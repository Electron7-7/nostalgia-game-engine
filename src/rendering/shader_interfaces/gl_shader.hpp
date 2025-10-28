#ifndef GL_SHADER_H
#define GL_SHADER_H

#include "shader_interface.hpp"

class GLShader : public ShaderInterface
{
public:
    virtual ~GLShader() = default;

    bool CompileShader(const std::string& VertexShaderCode, const std::string& FragmentShaderCode);
    bool IsValid() const final;

    void SetUniform(const std::string& Name, int Value) const final;
    void SetUniform(const std::string& Name, float Value) const final;
    void SetUniform(const std::string& Name, glm::vec2 Value) const final;
    void SetUniform(const std::string& Name, glm::vec3 Value) const final;
    void SetUniform(const std::string& Name, glm::vec4 Value) const final;
    void SetUniform(const std::string& Name, glm::mat3 Value) const final;
    void SetUniform(const std::string& Name, glm::mat4 Value) const final;

private:
    bool GLShaderErrorHandler(const ID& ID, bool IsLinkingInsteadOfCompiling = false) const;
    bool GLShaderClear(const ID& ID);
};

#endif // GL_SHADER_H
