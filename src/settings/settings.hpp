#ifndef SETTINGS_H
#define SETTINGS_H

#include "common/fwd.hpp"

#include <glm/fwd.hpp>

namespace Settings
{
    namespace Engine
    {
        extern int   TickRate;
        extern float TickInterval();
        extern int   GraphicsBackend;
        extern int   WindowingBackend;
    }

    namespace World
    {
        extern const glm::vec3& Up();
        extern const glm::vec3& Right();
        extern const glm::vec3& Front();

        extern bool UpdateOrientation(const ID& GraphicsLibraryID);
    }

    namespace Window
    {
        extern int Width;
        extern int Height;
        extern int XPosition;
        extern int YPosition;
        extern int FullscreenWidth;
        extern int FullscreenHeight;
        extern int FullscreenXPosition;
        extern int FullscreenYPosition;
        extern bool Fullscreen;
        extern const char* Name;
    }

    namespace Player
    {
        extern float FOV;
        extern float ViewCutoffNear;
        extern float ViewCutoffFar;
        extern bool  RawMouseMotion;
        extern float MouseSensitivity;
        extern float MouseSensitivityScale;
        extern float MovementSpeed;
    }

    namespace Graphics
    {
        extern bool GlobalWireframe;
    }
}

#endif // SETTINGS_H
