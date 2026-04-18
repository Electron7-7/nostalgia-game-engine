#ifndef OPENGL_RENDERER_API_H
#define OPENGL_RENDERER_API_H

#include <Nostalgia/rendering/renderer_api.hpp>

class OpenGLRendererAPI final : public RendererAPI
{
public:
    bool Init() final;
    void Shutdown() final;
    bool IsRunning() final;

    void SetViewport(Farg<Position2D>, Farg<Size2D>) final;
    void SetViewport(int, int, int, int) final;
    void SetClearColor(double, double, double, double) final;
    void SetClearColor(Farg<glm::vec4>) final;
    void SetClearColor(Farg<ColorRGBA>) final;
    void SetLineWidth(float) override;
    void SetFramebufferSRGB(bool) const final;
    void SetWireframe(bool inValue) const final;
    void SetBlend(bool) const final;
    void SetDepthMask(bool isEnabled) const final;

    ColorRGBA GetClearColor() final;
    float GetLineWidth() final;
    bool GetWireframe() const final;
    bool GetBlend() const final;
    Shared<Image> GetFullScreenshot() const final;

    void SetLight_TempBlinnPhongSolution(Shared<Light3D>) final;

    virtual bool BindTexture(Shared<Texture>, uint) const final;
    virtual bool BindTexture(Shared<TextureBuffer>, uint) const final;
    virtual void UnbindTexture(texture_units) const final;

    ID AddShader(Shared<Shader>, ID) final;
    Shared<Shader> GetShader(ID) final;
    Error RemoveShader(ID) final;

    void DrawText(Sarg, Shared<Font>, glm::vec2, Farg<glm::vec2>) final;
    void DrawIndexed(Shared<VertexArray>, uint = 0, uint = 0) final;
    void Clear() final;

private:
    mutable RMutex mTextMutex{}, mShadersMutex{}, mClearColorMutex{};
    std::unordered_map<ID, Shared<Shader>> mShaders{};
    std::array<double, 4> mClearColor{1.0f, 0.2f, 0.8f, 1.0f};
    bool mIsRunning{false};
    uint mTextVAO{0};
    uint mTextVBOs[2]{0, 0};
};

#endif // OPENGL_RENDERER_H
