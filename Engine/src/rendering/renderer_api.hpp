#ifndef RENDERER_API_H
#define RENDERER_API_H

#include "fwd/rendering.hpp"
#include "fwd/things.hpp"
#include "fwd/math.hpp"
#include "core/id.hpp"
#include "core/error.hpp"
#include "core/smart_pointers.hpp"
#include <glm/fwd.hpp>

enum class GraphicsAPI
{
    None,
    OpenGL,
};

class RendererAPI
{
public:
    using texture_units = std::initializer_list<uint>;

    static GraphicsAPI GetAPI() { return sAPI; }
    static Unique<RendererAPI> Activate();

    virtual ~RendererAPI() = default;

    virtual bool Init() = 0;
    virtual void Shutdown() = 0;

    virtual void SetViewport(Farg<Position2D> inPosition, Farg<Scale2D> inSize) = 0;
    virtual void SetViewport(int XPosition, int YPosition, int Width, int Height) = 0;
    virtual void SetClearColor(float Red, float Green, float Blue, float Alpha) = 0;
    virtual void SetClearColor(const glm::vec4& Color) = 0;
    virtual void SetLineWidth(float Width) = 0;
    virtual void SetFramebufferSRGB(bool inValue) const = 0;
    virtual void SetWireframe(bool inValue) const = 0;

    virtual void SetLight_TempBlinnPhongSolution(light_t* inLight) = 0;

    virtual void BindTexture(Shared<TextureBuffer> inTexture, uint inTextureUnit) const = 0;
    virtual void BindTexture(Shared<TextureBuffer> inTexture, texture_units) const = 0;
    virtual void UnbindTexture(texture_units inTextureUnits) const = 0;

    virtual ID AddShader(Shared<Shader> inShader, ID inID = ID::Invalid) = 0;
    virtual Shared<Shader> GetShader(ID inID) = 0;
    virtual Error RemoveShader(ID inID) = 0;

    virtual void DrawIndexed(Shared<VertexArray>, uint IndexCount = 0) = 0;
    virtual void Clear() = 0;

private:
    inline static GraphicsAPI sAPI{GraphicsAPI::OpenGL};
};

#endif // RENDERER_API_H
