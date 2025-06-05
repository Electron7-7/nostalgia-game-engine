#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

// I'm assuming that Valve used their RenderManager to delegate rendering to the various renderers (seeing as it's in src/app). I kind of
// want to keep all the abstract rendering code contained to one "renderer" class, so I'll be taking many liberties with the structure
// of this guy.

#include "manager.hpp"
#include "engine/rendering/camera_property.hpp"

class RenderManager : public Manager<>
{
public:
    // Inherited from _Manager
    virtual bool Init();
    virtual TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    virtual void Update();
    virtual TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);
    virtual void Shutdown();

    // Prototype functions
    bool prototype_SetFullscreen(bool EnableFullscreen);

    // "Try" functions: will attempt to do something and will return true if successful and false if unsuccessful
    bool try_SetActiveCameraProperty(CameraProperty* NewCameraProperty);

    // Variables related to the main window
    static bool is_main_window_fullscreen;
    static bool center_main_window_if_windowed;
    static const char* main_WindowName;
    static int main_WindowWidth;
    static int main_WindowHeight;
    static int main_WindowPositionX;
    static int main_WindowPositionY;

private:
    // Set the current projection matrix
    void SetProjectionMatrix(int Width, int Height, float FOV);

    // Set the current orthographic matrix
    void SetOrthoMatrix(int Width, int Height);

    // Draws the world
    void RenderWorld();

    // Note: I'm not currently planning on implementing multiplayer, but idk, maybe following Source's source code will make multiplayer easier?
    // Done completely client-side, want total smoothness, so simulate at render interval
    // void UpdateLocalPlayerCamera();

    CameraProperty default_FallbackMainCameraProperty;
    CameraProperty* current_CameraProperty = &default_FallbackMainCameraProperty;
};

// Singleton accessor
extern RenderManager* global_RenderManager;

#endif // RENDER_MANAGER_H
