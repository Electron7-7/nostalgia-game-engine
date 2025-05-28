#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include "manager.hpp"
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

    void RenderWorldInrect(int X, int Y, int Width, int Height);
    void RenderWorldFullscreen();

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

    bool render_world_fullscreen;
    int render_x;
    int render_y;
    int render_width;
    int render_height;
};

// Singleton accessor
extern RenderManager* global_RenderManager;

#endif // RENDER_MANAGER_H