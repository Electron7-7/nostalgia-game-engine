#ifndef OPENGL_RENDERER_API_H
#define OPENGL_RENDERER_API_H

#include "rendering/renderer_api.hpp"
#include "math/containers.hpp"
#include <unordered_map>

class OpenGLRendererAPI final : public RendererAPI
{
public:
    bool Init() final;
    void Shutdown() final;

    void SetViewport(Farg<Position2D>, Farg<Scale2D>) final;
    void SetViewport(int, int, int, int) final;
    void SetClearColor(float, float, float, float) final;
    void SetClearColor(Farg<glm::vec4>) final;
    void SetLineWidth(float) override;
    void SetFramebufferSRGB(bool) const final;
    void SetWireframe(bool inValue) const final;

    void SetLight_TempBlinnPhongSolution(light_t*) final;

    virtual void BindTexture(Shared<Texture>, uint) const final;
    virtual void BindTexture(Shared<Texture>, texture_units) const final;
    virtual void UnbindTexture(texture_units) const final;

    ID AddShader(Shared<Shader>, ID) final;
    Shared<Shader> GetShader(ID) final;
    Error RemoveShader(ID) final;

    ID AddFrameBuffer(Shared<FrameBuffer>, ID) final;
    Shared<FrameBuffer> GetFrameBuffer(ID) final;
    Error RemoveFrameBuffer(ID inID) final;

    void DrawIndexed(Farg<Shared<Camera3D>>, Shared<Mesh>, uint) final;
    void Clear() final;

private:
    std::unordered_map<ID, Shared<Shader>> mShaders{};
    std::unordered_map<ID, Shared<FrameBuffer>> mFramebuffers{};
    std::array<float, 4> mClearColor{1.0f, 0.2f, 0.8f, 1.0f};
    Position2D mViewportPosition{0, 0};
    Scale2D    mViewportSize{1280, 720};
};

enum class DebugMessageSeverityFilter
{
    None = 0,
    Notification,
    Low,
    Medium,
    High,
};

extern bool gPrintDrawLogs;
extern DebugMessageSeverityFilter gOpenGLMessageFilter;

#endif // OPENGL_RENDERER_H
