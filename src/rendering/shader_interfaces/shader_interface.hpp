#ifndef SHADER_INTERFACE_H
#define SHADER_INTERFACE_H

#include <string>
#include <glm/glm.hpp>

namespace Shaders
{
    constexpr unsigned int BlinnPhong = 0;
    constexpr unsigned int Fullbright = 1;
    constexpr unsigned int Skybox     = 2;
    constexpr unsigned int Fonts      = 3;
}

class ShaderInterface
{
public:
    virtual ~ShaderInterface() = default;

    virtual bool IsValid() const = 0;

    virtual void SetUniform(const std::string& Name, int Value) const = 0;
    virtual void SetUniform(const std::string& Name, float Value) const = 0;
    virtual void SetUniform(const std::string& Name, glm::vec2 Value) const = 0;
    virtual void SetUniform(const std::string& Name, glm::vec3 Value) const = 0;
    virtual void SetUniform(const std::string& Name, glm::vec4 Value) const = 0;
    virtual void SetUniform(const std::string& Name, glm::mat3 Value) const = 0;
    virtual void SetUniform(const std::string& Name, glm::mat4 Value) const = 0;

    unsigned int ID() { return m_ID; }

protected:
    unsigned int m_ID = 0;
    bool m_IsCompiled = false;
    bool m_IsBound    = false;
};

#endif // SHADER_INTERFACE_H
