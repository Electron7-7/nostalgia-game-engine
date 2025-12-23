#ifdef FWD_DCL
    class RenderManager;
    extern RenderManager* g_pRenderManager;
#elif !defined RENDER_MANAGER_H
#define RENDER_MANAGER_H

#define FWD_DCL
#   include "rendering/renderer_api.hpp"
#undef  FWD_DCL

#include "manager.hpp"

enum class ShaderDebugOutput : ushort
{
    All = 0,
    VertexColors = 1,
    VertexNormals = 2,
    VertexUVs = 3,
};

inline ushort gShaderDebugOuptut{static_cast<ushort>(ShaderDebugOutput::All)};

class RenderManager : public Manager
{
public:
    consteval const char* DebugName() final { return "RenderManager"; }

    bool Init() final;
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall) final;
    void Update() final;
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall) final;
    void Shutdown() final;

    void Event(AppEvent*) final;

    void SetAutomaticWindowClear(bool);

    const Unique<RendererAPI>& GetAPI() const;

private:
    Unique<RendererAPI> mRendererAPI{nullptr};
    bool mCanClearWindow{true};
};

// Singleton accessor
extern RenderManager* g_pRenderManager;

#endif // RENDER_MANAGER_H
