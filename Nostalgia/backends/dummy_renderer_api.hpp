#ifndef DUMMY_RENDERER_API_H
#define DUMMY_RENDERER_API_H

#include "opengl/gl_shader.hpp"
#include "rendering/renderer_api.hpp"
#include "things/resources/image.hpp"

class DummyRendererAPI final : public RendererAPI
{
    void Init() final {}
    void Shutdown() final {}
    void DrawFrame() final {}
    void AddToQueue(Unique<RenderCommand>) final {}
    void SetViewport(Farg<Position2D>, Farg<Size2D>) final {}
    void SetViewport(int, int, int, int) final {}
    void SetClearColor(Farg<ColorRGBA>) final {}
    void SetLineWidth(float) final {}
    void SetFramebufferSRGB(bool) const final {}
    void SetWireframe(bool) const final {}
    void SetBlend(bool) const final {}
    void SetDepthMask(bool) const final {}

    ColorRGBA GetClearColor() final { return {}; }
    float GetLineWidth() final { return 0; }
    bool GetWireframe() const final { return false; }
    bool GetBlend() const final { return false; }
    Shared<Image> GetFullScreenshot() const final { return MakeShared<Image>(); }

    void SetLight_TempBlinnPhongSolution(Shared<Light3D>) final {}

    void BindFramebuffer(Shared<FrameBuffer>) const final {}

    bool BindTexture(Shared<Texture>, uint) const final { return false; }
    bool BindTexture(Shared<TextureBuffer>, uint) const final { return false; }
    void UnbindTexture(texture_units) const final {}

    ID AddShader(Shared<Shader>, ID) final { return ID::Invalid; }
    Shared<Shader> GetShader(ID) final { return MakeShared<GLShader>(); }
    Error RemoveShader(ID) final { return FAILED; }

    void DrawText(Sarg, Shared<Font>, glm::vec2, Farg<glm::vec2>) final {}
    void DrawIndexed(Shared<VertexArray>, uint, uint) final {}
    void Clear() final {}
};

#endif // DUMMY_RENDERER_API_H
