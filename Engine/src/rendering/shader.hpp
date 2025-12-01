#ifndef SHADER_H
#define SHADER_H

#include "core/id.hpp"
#include "core/type_helpers.hpp"

#include <glm/fwd.hpp>

namespace Shaders
{
    ConstexprID BlinnPhong {"BlinnPhong"};
    ConstexprID Fullbright {"Fullbright"};
    ConstexprID Skybox     {"Skybox"};
    ConstexprID Fonts      {"Fonts"};
}

class IShader
{
public:
    virtual ~IShader() = default;

    virtual void SetUniform(FARG(std::string) inName, FARG(int) inValue)       const = 0;
    virtual void SetUniform(FARG(std::string) inName, FARG(float) inValue)     const = 0;
    virtual void SetUniform(FARG(std::string) inName, FARG(glm::vec2) inValue) const = 0;
    virtual void SetUniform(FARG(std::string) inName, FARG(glm::vec3) inValue) const = 0;
    virtual void SetUniform(FARG(std::string) inName, FARG(glm::vec4) inValue) const = 0;
    virtual void SetUniform(FARG(std::string) inName, FARG(glm::mat3) inValue) const = 0;
    virtual void SetUniform(FARG(std::string) inName, FARG(glm::mat4) inValue) const = 0;

    FARG(ID) id()         const { return mID; }
    bool     IsCompiled() const { return mIsCompiled; }
    bool     IsBound()    const { return mIsBound; }

protected:
    ID mID{ID::Invalid};
    bool mIsCompiled{false};
    bool mIsBound{false};
};

#endif // SHADER_H
