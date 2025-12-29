#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include "managers/manager.hpp"
#include "fwd/rendering.hpp"
#include "core/smart_pointers.hpp"

enum class ShaderDebugOutput : ushort
{
    All = 0,
    VertexColors = 1,
    VertexNormals = 2,
    VertexUVs = 3,
};

inline ushort gShaderDebugOutput{static_cast<ushort>(ShaderDebugOutput::All)};

class RenderManager : public Manager
{
public:
    consteval const char* DebugName() final { return "RenderManager"; }

    bool Init() final;
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall) final;
    void Update() final;
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall) final;
    void Shutdown() final;

    void SetAutomaticWindowClear(bool);

    const Unique<RendererAPI>& GetAPI() const;

private:
    Unique<RendererAPI> mRendererAPI{nullptr};
    bool mCanClearWindow{true};
};

// Singleton accessor
extern RenderManager* g_pRenderManager;

#endif // RENDER_MANAGER_H
