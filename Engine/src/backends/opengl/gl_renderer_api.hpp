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

    void SetLight_TempBlinnPhongSolution(Shared<Light3D>) final;

    virtual void BindTexture(Shared<TextureBuffer>, uint) const final;
    virtual void BindTexture(Shared<TextureBuffer>, texture_units) const final;
    virtual void UnbindTexture(texture_units) const final;

    ID AddShader(Shared<Shader>, ID) final;
    Shared<Shader> GetShader(ID) final;
    Error RemoveShader(ID) final;

    void DrawIndexed(Shared<VertexArray>, uint) final;
    void DrawSkybox(Shared<VertexArray>) final;
    void Clear() final;

private:
    std::unordered_map<ID, Shared<Shader>> mShaders{};
    std::array<float, 4> mClearColor{1.0f, 0.2f, 0.8f, 1.0f};
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
