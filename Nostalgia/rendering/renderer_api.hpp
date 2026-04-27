#ifndef RENDERER_API_H
#define RENDERER_API_H

#include <Nostalgia/fwd/rendering.hpp>
#include <Nostalgia/fwd/things.hpp>

class RendererAPI
{
public:
    enum GraphicsAPI : int
    {
        NONE = 0,
        OPENGL,
    };

    enum class ShaderDebugOutput : ushort
    {
        All = 0,
        VertexColors = 1,
        VertexNormals = 2,
        VertexUVs = 3,
    };

    inline static constexpr char s_cAPIWarningMessage[]{"RendererAPI::CurrentAPI() == NONE"};
    inline static ushort sShaderDebugOutput{static_cast<ushort>(ShaderDebugOutput::All)};

    using texture_units = std::initializer_list<uint>;

    static bool ActivateInstance(GraphicsAPI);
    static void DeactivateInstance();
    static bool HasActiveInstance();
    static RendererAPI* Get();
    static GraphicsAPI CurrentAPI();
    static LockGuard<RMutex> GetLock();

    virtual ~RendererAPI() = default;

    virtual void Init() = 0;
    virtual void Shutdown() = 0;

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

    virtual void BindFramebuffer(Shared<FrameBuffer> inBuffer = nullptr) const = 0;

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
    static Unique<RendererAPI> ms_pInstance;
    inline static RMutex m_sInstanceMutex{};
    inline static GraphicsAPI m_sType{NONE};
    inline static bool m_sInstanceActive{false};
};

#endif // RENDERER_API_H
