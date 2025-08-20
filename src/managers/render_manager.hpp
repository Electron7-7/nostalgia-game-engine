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
    virtual bool Init();
    virtual ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    virtual void Update();
    virtual ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);
    virtual void Shutdown();

    // "Try" functions: will attempt to do something and will return true if successful and false if unsuccessful
    // (these should be guaranteed not to be important enough to cause a crash or other major problems)
    // bool try_SetActiveCameraProperty(CameraProperty* NewCameraProperty);

private:
    void SetProjectionMatrix(int Width, int Height, float FOV);
    void SetOrthoMatrix(int Width, int Height);
};

// Singleton accessor
extern RenderManager* global_RenderManager;

#endif // RENDER_MANAGER_H
