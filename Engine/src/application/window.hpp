#ifndef WINDOW_H
#define WINDOW_H

#include "fwd.hpp"
#include "common/sanity.hpp"
#include "core/game_loop.hpp"
#include "core/error.hpp"

#include <cassert>
#include <string>
#include <memory>
#include <vector>

enum NativeWindowType
{
    NATIVE_GLFW_WINDOW,
};

class IWindow : public Updated
{
public:
    enum Vsync
    {
        VSYNC_OFF,      // le chud: "Nooooo! You must endure input lag to avoid screen tearing!" le gigachad: "The screen now looks like cyberpunk hacking"
        VSYNC_ON,       // Not implemented
        VSYNC_ADAPTIVE, // Not implemented
    };

    enum MouseMode
    {
        MOUSE_MODE_VISIBLE,  // GLFW_CURSOR_NORMAL
        MOUSE_MODE_HIDDEN,   // GLFW_CURSOR_HIDDEN
        MOUSE_MODE_CAPTURED, // GLFW_CURSOR_CAPTURED
        MOUSE_MODE_DISABLED, // GLFW_CURSOR_DISABLED
    };

    enum WindowMode
    {
        WINDOW_MODE_WINDOWED,
        WINDOW_MODE_FULLSCREEN,
        WINDOW_MODE_BORDERLESS, // Not implemented
    };

    struct WindowProperties
    {
        std::string title{""};
        uint width{0}, height{0};
        uint x_pos{0}, y_pos{0};
        Vsync vsync{VSYNC_OFF};
        WindowMode window_mode{WINDOW_MODE_WINDOWED};
        MouseMode mouse_mode{MOUSE_MODE_VISIBLE};

        constexpr bool operator==(const WindowProperties& other) const
        {
            return title == other.title &&
                   width == other.width &&
                   height == other.height &&
                   x_pos == other.x_pos &&
                   y_pos == other.y_pos &&
                   vsync == other.vsync &&
                   window_mode == other.window_mode &&
                   mouse_mode == other.mouse_mode;
        }

        WindowProperties(StringArg inTitle = {"Nostalgia Engine"},
            uint inWidth = 1280,  uint inHeight = 720,
            uint inXPosition = 0, uint inYPosition = 0):
                title{inTitle},
                width{inWidth}, height{inHeight},
                x_pos{inXPosition}, y_pos{inYPosition},
                vsync{VSYNC_OFF}, window_mode{WINDOW_MODE_WINDOWED}, mouse_mode{MOUSE_MODE_VISIBLE}
                {}
    };

    virtual ~IWindow() = default;

    virtual void Update() = 0;

    virtual Error UpdateAllProperties() = 0;
    virtual Error SetVsync(Vsync) = 0;
    virtual Error SetMouseMode(MouseMode) = 0;
    virtual Error SetWindowMode(WindowMode) = 0;
    virtual void* GetNativeWindow() const = 0;
    virtual NativeWindowType GetNativeWindowType() const = 0; // Probably not necessary
    virtual const std::unique_ptr<Monitor>& GetPrimaryMonitor() const = 0;

    const char* GetTitle()     const { return mData.title.data(); }
    uint GetWidth()            const { return mData.width; }
    uint GetHeight()           const { return mData.height; }
    int GetXPosition()         const { return mData.x_pos; }
    int GetYPosition()         const { return mData.y_pos; }
    Vsync GetVsync()           const { return mData.vsync; }
    MouseMode GetMouseMode()   const { return mData.mouse_mode; }
    WindowMode GetWindowMode() const { return mData.window_mode; }
    bool IsFullscreen()        const { return GetWindowMode() == WINDOW_MODE_FULLSCREEN; }
    Error GetInitStatus()      const { return mInitStatus; }

    uint GetFullscreenMonitorIndex()
    {
        assert(m_sMonitors.size() > 0 && "IWindow::m_sMonitors.size() <= 0");
        if(mFullscreenMonitorIndex >= m_sMonitors.size())
            { mFullscreenMonitorIndex = 0; }
        return mFullscreenMonitorIndex;
    }

    const std::unique_ptr<Monitor>& GetFullscreenMonitor()
    { return m_sMonitors.at(GetFullscreenMonitorIndex()); }

    Error SetFullscreenMonitor(uint MonitorIndex)
    {
        if(MonitorIndex < m_sMonitors.size())
        {
            mFullscreenMonitorIndex = MonitorIndex;
            if(IsFullscreen())
                { UpdateAllProperties(); }
            return OK;
        }
        return ERR_INDEX_OUT_OF_BOUNDS;
    }

    template<typename T>
        requires std::derived_from<T,IWindow>
    static std::unique_ptr<IWindow> CreateWindow(const WindowProperties& inProperties = {})
    { return std::make_unique<T>(inProperties); }

    static std::vector<Monitor> GetMonitors();

#pragma message("TODO: SetEventCallback")

protected:
    Error mInitStatus{FAILED};
    WindowProperties mData{};
    uint mFullscreenMonitorIndex{0};

    virtual Error InitializeCallbacks() = 0;

    static std::vector<std::unique_ptr<Monitor>> m_sMonitors;
};

#endif // WINDOW_H
