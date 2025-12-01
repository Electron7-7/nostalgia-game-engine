#ifndef OPENGL_RENDERER_API_H
#define OPENGL_RENDERER_API_H

#include "rendering/renderer_api.hpp"

class OpenGLRendererAPI : public IRendererAPI
{
public:
    virtual void Init() override;
    virtual void SetViewport(uint XPosition, uint YPosition, uint Width, uint Height) override;
    virtual void SetClearColor(float Red, float Green, float Blue, float Alpha) override;
    virtual void SetClearColor(const glm::vec4& Color) override;
    virtual void Clear() override;

    // virtual void DrawIndexed(const VertexArray&, uint IndexCount = 0, int BaseIndex = -1);
    // virtual void DrawLines(const VertexArray&, uint VertexCount = 0);
    virtual void SetLineWidth(float Width) override;
};

#endif // OPENGL_RENDERER_H
