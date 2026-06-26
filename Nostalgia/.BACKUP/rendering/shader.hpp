#ifndef SHADER_H
#define SHADER_H

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

    virtual bool CompileShader(Sarg inVertexShader, Sarg inFragmentShader) = 0;

    virtual void Bind()   const = 0;
    virtual void Unbind() const = 0;

    virtual void SetUniform(Sarg inName, int inValue)             const = 0;
    virtual void SetUniform(Sarg inName, uint inValue)            const = 0;
    virtual void SetUniform(Sarg inName, float inValue)           const = 0;
    virtual void SetUniform(Sarg inName, Farg<glm::vec2> inValue) const = 0;
    virtual void SetUniform(Sarg inName, Farg<glm::vec3> inValue) const = 0;
    virtual void SetUniform(Sarg inName, Farg<glm::vec4> inValue) const = 0;
    virtual void SetUniform(Sarg inName, Farg<glm::mat3> inValue) const = 0;
    virtual void SetUniform(Sarg inName, Farg<glm::mat4> inValue) const = 0;

    Farg<ID> id()         const { return mID; }
    bool     IsCompiled() const { return mIsCompiled; }

    static Shared<Shader> CreateDummy();
    static Shared<Shader> Create();

protected:
    ID mID{};
    bool mIsCompiled{false};
};

class DummyShader final : public Shader
{
public:
    DummyShader() = default;

    virtual bool CompileShader(Sarg, Sarg) final { return false; }

    virtual void Bind()   const final {}
    virtual void Unbind() const final {}

    virtual void SetUniform(Sarg, int)             const final {}
    virtual void SetUniform(Sarg, uint)            const final {}
    virtual void SetUniform(Sarg, float)           const final {}
    virtual void SetUniform(Sarg, Farg<glm::vec2>) const final {}
    virtual void SetUniform(Sarg, Farg<glm::vec3>) const final {}
    virtual void SetUniform(Sarg, Farg<glm::vec4>) const final {}
    virtual void SetUniform(Sarg, Farg<glm::mat3>) const final {}
    virtual void SetUniform(Sarg, Farg<glm::mat4>) const final {}
};

#endif // SHADER_H
