#ifndef OPENGL_RENDERER_API_H
#define OPENGL_RENDERER_API_H

#include "rendering/renderer_api.hpp"

#include <unordered_map>

class OpenGLRendererAPI final : public RendererAPI
{
public:
    bool Init() final;
    void Shutdown() final;

    void SetViewport(uint, uint, uint, uint) final;
    void SetClearColor(float, float, float, float) final;
    void SetClearColor(Farg<glm::vec4>) final;
    void SetLineWidth(float) override;
    void SetFramebufferSRGB(bool) const final;
    void SetWireframe(bool inValue) const final;

    void SetLight_TempBlinnPhongSolution(light_t*) final;

    ID AddShader(Shared<Shader>, ID) final;
    Shared<Shader> GetShader(ID) final;
    ID AddViewport(Farg<Viewport>, ID) final;
    Viewport& GetViewport(ID) final;
    Error RemoveViewport(ID) final;
    ID GenerateFrameBuffer(ID inID) final;
    Shared<FrameBuffer>& GetFrameBuffer(ID inID) final;
    Error RemoveFrameBuffer(ID inID) final;

    void DrawIndexed(Shared<VertexArray>, uint, RenderLayers) final;
    void DrawLines(Shared<VertexArray>, uint, RenderLayers) final;
    void Clear() final;

    Farg<std::unordered_map<ID, Viewport>> GetViewports() const noexcept { return mViewports; }

private:
    std::unordered_map<ID, Shared<Shader>> mShaders{};
    std::unordered_map<ID, Viewport> mViewports{};
    std::unordered_map<ID, Shared<FrameBuffer>> mFramebuffers{};
};

extern bool gGLViewportUseDirectFromViewport;
extern bool gPrintDrawLogs;
extern bool gPrintViewportAddedLogs;

#endif // OPENGL_RENDERER_H
