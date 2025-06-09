#ifndef SHADER_INTERFACE_H
#define SHADER_INTERFACE_H

#include <string>
#include <glm/glm.hpp>

namespace Shaders
{
    constexpr unsigned int BLINN_PHONG = 0;
    constexpr unsigned int SKYBOX      = 1;
    constexpr unsigned int FULLBRIGHT  = 2;
    constexpr unsigned int FONTS       = 3;
}

class ShaderInterface
{
public:
    virtual ~ShaderInterface() = default;

    virtual bool IsValid() const = 0;
    virtual void Bind() const = 0;
    virtual void SetUniform(const std::string& Name, int Value) const = 0;
    virtual void SetUniform(const std::string& Name, float Value) const = 0;
    virtual void SetUniform(const std::string& Name, glm::vec2 Value) const = 0;
    virtual void SetUniform(const std::string& Name, glm::vec3 Value) const = 0;
    virtual void SetUniform(const std::string& Name, glm::mat4 Value) const = 0;

protected:
    unsigned int _id = 0;
    bool shader_compiled_properly = false;
};

#endif // SHADER_INTERFACE_H
