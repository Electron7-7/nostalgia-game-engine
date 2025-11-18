#ifndef SHADER_INTERFACE_H
#define SHADER_INTERFACE_H

#include "core/id.hpp"

#include <string>
#include <glm/glm.hpp>

namespace Shaders
{
    constexpr ID BlinnPhong {"BlinnPhong"};
    constexpr ID Fullbright {"Fullbright"};
    constexpr ID Skybox     {"Skybox"};
    constexpr ID Fonts      {"Fonts"};
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

    const ID& id() { return mID; }

protected:
    ID mID{ID::Invalid};
    bool mIsCompiled{false};
    bool mIsBound{false};
};

#endif // SHADER_INTERFACE_H
