#ifndef WINDOW_SETTINGS_H
#define WINDOW_SETTINGS_H

#include "renderer/window_info.hpp"

namespace Settings
{
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
        const position_t& FramebufferPosition();
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
        void setFramebufferPosition(const position_t&);
    }
}

#endif // WINDOW_SETTINGS_H
