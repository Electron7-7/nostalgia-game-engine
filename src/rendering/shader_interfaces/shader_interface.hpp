#ifndef SHADER_INTERFACE_H
#define SHADER_INTERFACE_H

#include <string>
#include <glm/glm.hpp>

namespace Shaders
{
    constexpr unsigned int SAFETY      = 0; // Used when deleting a shader that is currently bound
    constexpr unsigned int BLINN_PHONG = 1;
    constexpr unsigned int SKYBOX      = 2;
    constexpr unsigned int FULLBRIGHT  = 3;
    constexpr unsigned int FONTS       = 4;
}

class ShaderInterface
{
public:
    virtual ~ShaderInterface() = default;

    virtual bool IsValid() const = 0;
    virtual void Bind()   = 0;
    virtual void Unbind() = 0;
    virtual void Delete() = 0;
    virtual void SetUniform(const std::string& Name, int Value) const = 0;
    virtual void SetUniform(const std::string& Name, float Value) const = 0;
    virtual void SetUniform(const std::string& Name, glm::vec2 Value) const = 0;
    virtual void SetUniform(const std::string& Name, glm::vec3 Value) const = 0;
    virtual void SetUniform(const std::string& Name, glm::mat4 Value) const = 0;

protected:
    unsigned int _id = 0;
    bool is_compiled = false;
    bool is_bound = false;
};

#endif // SHADER_INTERFACE_H
