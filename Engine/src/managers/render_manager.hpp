#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include "manager.hpp"

#include <sys/types.h>

class RenderManager : public Manager
{
public:
    consteval const char* DebugName() { return "RenderManager"; }

    bool Init();
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    void Update();
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);
    void Shutdown();

    void WindowResized(uint Width, uint Height);
};

// Singleton accessor
extern RenderManager* g_pRenderManager;

#endif // RENDER_MANAGER_H
