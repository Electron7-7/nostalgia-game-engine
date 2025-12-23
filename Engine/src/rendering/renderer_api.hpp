#ifdef FWD_DCL
enum class GraphicsAPI;
class RendererAPI;
#elif !defined RENDERER_API_H
#define RENDERER_API_H

#define FWD_DCL
#   include "shader.hpp"
#   include "vertex_array.hpp"
#   include "frame_buffer.hpp"
#   include "things/actors/light.hpp"
#undef  FWD_DCL

#include "viewport.hpp"
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
    static GraphicsAPI GetAPI() { return sAPI; }
    static Unique<RendererAPI> Activate();

    virtual ~RendererAPI() = default;

    virtual bool Init() = 0;
    virtual void Shutdown() = 0;

    virtual void SetViewport(uint XPosition, uint YPosition, uint Width, uint Height) = 0;
    virtual void SetClearColor(float Red, float Green, float Blue, float Alpha) = 0;
    virtual void SetClearColor(const glm::vec4& Color) = 0;
    virtual void SetLineWidth(float Width) = 0;
    virtual void SetFramebufferSRGB(bool inValue) const = 0;
    virtual void SetWireframe(bool inValue) const = 0;

    virtual void SetLight_TempBlinnPhongSolution(light_t* inLight) = 0;

    virtual ID AddShader(Shared<Shader> inShader, ID inID = ID::Invalid) = 0;
    virtual Shared<Shader> GetShader(ID inID) = 0;
    virtual ID AddViewport(Farg<Viewport> inViewport, ID inID = ID::Invalid) = 0;
    virtual Viewport& GetViewport(ID inID) = 0;
    virtual Error RemoveViewport(ID inID) = 0;
    virtual ID GenerateFrameBuffer(ID inID = ID::Invalid) = 0;
    virtual Shared<FrameBuffer>& GetFrameBuffer(ID inID) = 0;
    virtual Error RemoveFrameBuffer(ID inID) = 0;

    virtual void DrawIndexed(Shared<VertexArray>, uint IndexCount = 0, RenderLayers = {}) = 0;
    virtual void DrawLines(Shared<VertexArray>, uint VertexCount = 0, RenderLayers = {}) = 0;
    virtual void Clear() = 0;

private:
    inline static GraphicsAPI sAPI{GraphicsAPI::OpenGL};
};

#endif // RENDERER_API_H
