#ifndef RENDERER_API_H
#define RENDERER_API_H

#include "core/type_helpers.hpp"

#include <glm/fwd.hpp>

enum class GraphicsAPI
{
    None,
    OpenGL,
};

class IRendererAPI
{
public:
    static GraphicsAPI GetAPI() { return sAPI; }
    static Unique<IRendererAPI> Activate();

    virtual ~IRendererAPI() = default;

    virtual void Init() = 0;
    virtual void SetViewport(uint XPosition, uint YPosition, uint Width, uint Height) = 0;
    virtual void SetClearColor(float Red, float Green, float Blue, float Alpha) = 0;
    virtual void SetClearColor(const glm::vec4& Color) = 0;
    virtual void Clear() = 0;

    // virtual void DrawIndexed(const VertexArray&, uint IndexCount = 0, int BaseIndex = -1);
    // virtual void DrawLines(const VertexArray&, uint VertexCount = 0);
    virtual void SetLineWidth(float Width) = 0;

private:
    inline static GraphicsAPI sAPI{GraphicsAPI::None};
};

#endif // RENDERER_API_H
