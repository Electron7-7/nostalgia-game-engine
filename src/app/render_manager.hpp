#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include "manager.hpp"
#include <backends/backends.hpp>
#include <backends/backend_manager.hpp>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

// Valve comment: "Physics property for entities"
class CameraProperty
{
public:
    CameraProperty();

    void GetForward(glm::vec3* ForwardVector);

    glm::vec3 origin;
    glm::quat quaternion;
    glm::vec3 velocity;
    glm::vec3 angular_velocity;

private:
    friend class RenderManager;
};

class RenderManager : public Manager<>
{
public:
    // Inherited from _Manager
    virtual bool Init();
    virtual TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    virtual void Update();
    virtual TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);
    virtual void Shutdown();

    // Property allocation
    CameraProperty* CreateCameraProperty();
    void DestroyCameraProperty(CameraProperty* Property);

    // TODO: Decide whether or not to keep this
    void RenderWorldInrect(int X, int Y, int Width, int Height);
    // void RenderWorldFullscreen();

    // Variables related to the main window
    static bool is_main_window_fullscreen;
    static bool center_main_window_if_windowed;
    static const char* main_WindowName;
    static int main_WindowWidth;
    static int main_WindowHeight;
    static int main_WindowPositionX;
    static int main_WindowPositionY;

private:
    // Set up a projection matrix for a 90 degree fov
    void SetupProjectionMatrix(int Width, int Height, float FOV);

    // Set up an orthographic projection matrix
    void SetupOrthoMatrix(int Width, int Height);

    // Sets up the camera
    void SetupCameraRenderState();

    // Draws the world
    void RenderWorld();

    // Note: I'm not currently planning on implementing multiplayer, but idk, maybe following Source's source code will make multiplayer easier?
    // Done completely client-side, want total smoothness, so simulate at render interval
    void UpdateLocalPlayerCamera();
};

// Singleton accessor
extern RenderManager* global_RenderManager;

#endif // RENDER_MANAGER_H