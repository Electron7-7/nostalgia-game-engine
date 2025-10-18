#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

// I'm assuming that Valve used their RenderManager to delegate rendering to the various renderers (seeing as it's in src/app). I kind of
// want to keep all the abstract rendering code contained to one "renderer" class, so I'll be taking many liberties with the structure
// of this guy.

// Secondary note: I actually might want to split up rendering between the game, the UI, the window/app, etc. The RenderManager will still
// be the delegator/controller/...manager but the more detailed functions should go to their respective renderers.

#include "manager.hpp"

class RenderManager : public Manager
{
public:
    constexpr const char* DebugName() { return "RenderManager"; }
    bool Init();
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    void Update();
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);
    void Shutdown();
};

// Singleton accessor
extern RenderManager* g_pRenderManager;

#endif // RENDER_MANAGER_H
