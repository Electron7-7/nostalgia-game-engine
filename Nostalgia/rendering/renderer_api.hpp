#ifndef RENDERER_API_H
#define RENDERER_API_H

#include <Nostalgia/fwd/rendering.hpp>
#include <Nostalgia/fwd/things.hpp>

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
    virtual bool IsRunning() = 0;

    virtual void SetViewport(Farg<Position2D> inPosition, Farg<Size2D> inSize) = 0;
    virtual void SetViewport(int XPosition, int YPosition, int Width, int Height) = 0;
    virtual void SetClearColor(Farg<ColorRGBA> Color) = 0;
    virtual void SetLineWidth(float Width) = 0;
    virtual void SetFramebufferSRGB(bool inValue) const = 0;
    virtual void SetWireframe(bool inValue) const = 0;
    virtual void SetBlend(bool isEnabled) const = 0;
    virtual void SetDepthMask(bool isEnabled) const = 0;

    virtual ColorRGBA GetClearColor() = 0;
    virtual float GetLineWidth() = 0;
    virtual bool GetWireframe() const = 0;
    virtual bool GetBlend() const = 0;
    virtual Shared<Image> GetFullScreenshot() const = 0;

    virtual void SetLight_TempBlinnPhongSolution(Shared<Light3D> inLight) = 0;

    virtual void BindFramebuffer(uint inFrameBuffer = 0) const = 0;

    virtual bool BindTexture(Shared<Texture> inTexture, uint inTextureUnit) const = 0;
    virtual bool BindTexture(Shared<TextureBuffer> inBuffer, uint inTextureUnit) const = 0;
    virtual void UnbindTexture(texture_units inTextureUnits) const = 0;

    virtual ID AddShader(Shared<Shader> inShader, ID inID = ID::Invalid) = 0;
    virtual Shared<Shader> GetShader(ID inID) = 0;
    virtual Error RemoveShader(ID inID) = 0;

    virtual void DrawText(Sarg inText,
        Shared<Font> inFont, glm::vec2 inPos, Farg<glm::vec2> inScale = {1.0f, 1.0f}) = 0;
    virtual void DrawIndexed(Shared<VertexArray>, uint IndexCount = 0, uint IndexOffset = 0) = 0;
    virtual void Clear() = 0;

private:
    inline static GraphicsAPI sAPI{GraphicsAPI::OpenGL};
};

#endif // RENDERER_API_H
