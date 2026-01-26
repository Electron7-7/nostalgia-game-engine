#ifndef SHADER_H
#define SHADER_H

#include <glm/fwd.hpp>

#pragma message("TODO: remove/move namespace 'Shaders'")
namespace Shaders
{
    inline constinit const uint BlinnPhong {ConstexprHash("BlinnPhong")};
    inline constinit const uint Fullbright {ConstexprHash("Fullbright")};
    inline constinit const uint SkyBox     {ConstexprHash("SkyBox")};
    inline constinit const uint Fast2D     {ConstexprHash("Fast2D")};
    inline constinit const uint Fonts      {ConstexprHash("Fonts")};
}

class Shader
{
public:
    virtual ~Shader() = default;

    virtual bool CompileShader(Farg<std::string> inVertexShader, Farg<std::string> inFragmentShader) = 0;

    virtual void Bind()   const = 0;
    virtual void Unbind() const = 0;

    virtual void SetUniform(Farg<std::string> inName, Farg<int> inValue)       const = 0;
    virtual void SetUniform(Farg<std::string> inName, Farg<float> inValue)     const = 0;
    virtual void SetUniform(Farg<std::string> inName, Farg<glm::vec2> inValue) const = 0;
    virtual void SetUniform(Farg<std::string> inName, Farg<glm::vec3> inValue) const = 0;
    virtual void SetUniform(Farg<std::string> inName, Farg<glm::vec4> inValue) const = 0;
    virtual void SetUniform(Farg<std::string> inName, Farg<glm::mat3> inValue) const = 0;
    virtual void SetUniform(Farg<std::string> inName, Farg<glm::mat4> inValue) const = 0;

    Farg<ID> id()         const { return mID; }
    bool     IsCompiled() const { return mIsCompiled; }

    static Shared<Shader> Create();

protected:
    ID mID{};
    bool mIsCompiled{false};
};

#endif // SHADER_H
