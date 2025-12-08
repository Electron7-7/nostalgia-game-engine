#ifndef WINDOW_H
#define WINDOW_H

#include "fwd.hpp"
#include "events/fwd.hpp"
#include "math/fwd.hpp"

#include "core/type_helpers.hpp"
#include "core/error.hpp"
#include "components/game_loop.hpp"
#include "common/sanity.hpp"

#include <cassert>
#include <string>
#include <memory>
#include <vector>
#include <glm/fwd.hpp>

#ifdef WAYLAND_DISPLAY
#   define WINDOW_SET_POSITION_DECLARATION
#   define WINDOW_SET_SCALE_DECLARATION
#   define WINDOW_SET_POSITION_DEFINITION(VAR_NAME) static Error __removed(const Position2D& VAR_NAME)
#   define WINDOW_SET_SCALE_DEFINITION(VAR_NAME)    static Error __removed(const Scale2D& VAR_NAME)
#else  // !WAYLAND_DISPLAY
#   define WINDOW_SET_POSITION_DECLARATION          Error SetPosition(const Position2D&) override;
#   define WINDOW_SET_SCALE_DECLARATION             Error SetScale(const Scale2D&) override;
#   define WINDOW_SET_POSITION_DEFINITION(CLASS, VAR_NAME) Error CLASS::SetPosition(const Position2D& VAR_NAME)
#   define WINDOW_SET_SCALE_DEFINITION(CLASS, VAR_NAME)    Error CLASS::SetScale(const Scale2D& VAR_NAME)
#endif // WAYLAND_DISPLAY

enum NativeWindowType
{
    NATIVE_GLFW_WINDOW,
};

class IWindow : public OnUpdate
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
        int  x_pos{0}, y_pos{0};
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
            int inXPosition = 0,  int inYPosition = 0):
                title{inTitle},
                width{inWidth}, height{inHeight},
                x_pos{inXPosition}, y_pos{inYPosition},
                vsync{VSYNC_OFF}, window_mode{WINDOW_MODE_WINDOWED}, mouse_mode{MOUSE_MODE_VISIBLE}
                {}
    };
    typedef WindowProperties Properties;

    virtual ~IWindow() = default;

    virtual void Update() override = 0;

#ifdef WAYLAND_DISPLAY
    virtual Error SetPosition(const Position2D&) { return UNIMPLEMENTED; }
    virtual Error SetScale(const Scale2D&)       { return UNIMPLEMENTED; }
#else // !WAYLAND_DISPLAY
    virtual Error SetPosition(const Position2D&) = 0;
    virtual Error SetScale(const Scale2D&)     = 0;
#endif // WAYLAND_DISPLAY

    virtual Error SetVsync(Vsync) = 0;
    virtual Error SetMouseMode(MouseMode) = 0;
    virtual Error SetWindowMode(WindowMode) = 0;

    virtual Position2D GetMousePosition() = 0;
    virtual Position2D GetLastMousePosition() = 0;
    virtual void* GetNativeWindow() const = 0;
    virtual NativeWindowType GetNativeWindowType() const = 0; // Probably not necessary
    virtual const std::unique_ptr<Monitor>& GetPrimaryMonitor() const = 0;
    virtual uint GetFullscreenMonitorIndex() = 0;
    virtual const std::unique_ptr<Monitor>& GetFullscreenMonitor() = 0;
    virtual Error SetFullscreenMonitor(uint MonitorIndex) = 0;

    const char* GetTitle()     const { return mData.title.data(); }
    Scale2D GetScale()         const;
    Position2D GetPosition()   const;
    uint GetWidth()            const { return mData.width; }
    uint GetHeight()           const { return mData.height; }
    int GetXPosition()         const { return mData.x_pos; }
    int GetYPosition()         const { return mData.y_pos; }
    Vsync GetVsync()           const { return mData.vsync; }
    MouseMode GetMouseMode()   const { return mData.mouse_mode; }
    WindowMode GetWindowMode() const { return mData.window_mode; }
    bool IsFullscreen()        const { return GetWindowMode() == WINDOW_MODE_FULLSCREEN; }
    Error GetInitStatus()      const { return mInitStatus; }
    FARG(WindowProperties) GetProperties() const { return mData; }

    template<typename T> requires std::derived_from<T,IWindow>
        static std::unique_ptr<IWindow> CreateWindow(const WindowProperties& inProperties = {})
        { return std::make_unique<T>(inProperties); }

    static std::vector<Monitor> GetMonitors();

protected:
    Error mInitStatus{FAILED};
    WindowProperties mData{};
    uint mFullscreenMonitorIndex{0};

    virtual Error InitializeCallbacks() = 0;
};

#endif // WINDOW_H
