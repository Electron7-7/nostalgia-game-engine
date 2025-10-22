#ifndef SETTINGS_H
#define SETTINGS_H

#include "common/fwd.hpp"
#include "window_info.hpp"

#include <glm/fwd.hpp>

namespace Settings
{
    namespace Engine
    {
        extern int   TickRate;
        extern float TickInterval();
        extern ID    GraphicsBackend;
        extern ID    WindowingBackend;
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
        const WindowInfo& Info();
        const WindowInfo& WindowedInfo();
        const WindowInfo& FullscreenInfo();
        const std::string& Name();
        const char* c_Name();
        bool Fullscreen();
        const position_t& Position();
        const scale_t& Size();
        const scale_t& FramebufferSize();
        void setAllInfo(const WindowInfo&);
        void setCurrentInfo(const WindowInfo&);
        void setWindowedInfo(const WindowInfo&);
        void setFullscreenInfo(const WindowInfo&);
        void setName(const std::string&);
        void setFullscreen(bool);
        // Returns the fullscreen status
        bool ToggleFullscreen();
        void setPosition(const position_t&);
        void setSize(const scale_t&);
        void setFramebufferSize(const scale_t&);
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
