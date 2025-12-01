#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include "rendering/fwd.hpp"

#include "manager.hpp"
#include "core/type_helpers.hpp"

#include <sys/types.h>

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

private:
    Unique<IRendererAPI> mRendererAPI{nullptr};
    bool mCanClearWindow{true};
};

// Singleton accessor
extern RenderManager* g_pRenderManager;

#endif // RENDER_MANAGER_H
