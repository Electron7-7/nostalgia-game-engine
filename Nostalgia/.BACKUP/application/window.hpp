#ifndef WINDOW_H
#define WINDOW_H

#include <Nostalgia/fwd/rendering.hpp>
#include <Nostalgia/application/monitor.hpp>

#ifdef WAYLAND_DISPLAY
#   define WINDOW_SET_POSITION_DECLARATION
#   define WINDOW_SET_SCALE_DECLARATION
#   define WINDOW_SET_POSITION_DEFINITION(VAR_NAME) static Error __removed(const Position2D<double>& VAR_NAME)
#   define WINDOW_SET_SCALE_DEFINITION(VAR_NAME)    static Error __removed(const Size2D<int>& VAR_NAME)
#else  // !WAYLAND_DISPLAY
#   define WINDOW_SET_POSITION_DECLARATION          Error SetPosition(const Position2D<double>&) override;
#   define WINDOW_SET_SCALE_DECLARATION             Error SetScale(const Size2D<int>&) override;
#   define WINDOW_SET_POSITION_DEFINITION(CLASS, VAR_NAME) Error CLASS::SetPosition(const Position2D<double>& VAR_NAME)
#   define WINDOW_SET_SCALE_DEFINITION(CLASS, VAR_NAME)    Error CLASS::SetScale(const Size2D<int>& VAR_NAME)
#endif // WAYLAND_DISPLAY

enum NativeWindowType : int
{
    NATIVE_GLFW_WINDOW,
};

class IWindow
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

        WindowProperties(const std::string& inTitle = {"Nostalgia Engine"},
            uint inWidth = 1280,  uint inHeight = 720,
            int inXPosition = 0,  int inYPosition = 0):
                title{inTitle},
                width{inWidth}, height{inHeight},
                x_pos{inXPosition}, y_pos{inYPosition},
                vsync{VSYNC_OFF}, window_mode{WINDOW_MODE_WINDOWED}, mouse_mode{MOUSE_MODE_VISIBLE}
            {}
    };

    typedef WindowProperties Properties;

    static Unique<IWindow> CreateNewWindow(Farg<WindowProperties> inProperties = {});
    static std::vector<Monitor> GetMonitors();

    virtual ~IWindow() = default;

    virtual bool Closed() const = 0;
    virtual bool IsMainWindow() const = 0;
    virtual void Close() = 0;
    virtual void Update() = 0;
    virtual Error SetPosition(Farg<Position2D<double>>) { return UNIMPLEMENTED; }
    virtual Error SetScale(Farg<Size2D<int>>)        { return UNIMPLEMENTED; }
    virtual Error SetVsync(Vsync) = 0;
    virtual Error SetMouseMode(MouseMode) = 0;
    virtual Error SetWindowMode(WindowMode) = 0;

    virtual Position2D<double> GetMousePosition() = 0;
    virtual Position2D<double> GetLastMousePosition() = 0;
    virtual void* GetNativeWindow() const = 0;
    virtual NativeWindowType GetNativeWindowType() const = 0; // Probably not necessary
    virtual const Unique<Monitor>& GetPrimaryMonitor() const = 0;
    virtual uint GetFullscreenMonitorIndex() = 0;
    virtual const Unique<Monitor>& GetFullscreenMonitor() = 0;
    virtual Error SetFullscreenMonitor(uint MonitorIndex) = 0;
    virtual Unique<GraphicsContext>& GetContext() = 0;

    virtual const char* GetTitle()           const = 0;
    virtual Size2D<int> GetScale()                const = 0;
    virtual Position2D<double> GetPosition()         const = 0;
    virtual uint GetWidth()                  const = 0;
    virtual uint GetHeight()                 const = 0;
    virtual int GetXPosition()               const = 0;
    virtual int GetYPosition()               const = 0;
    virtual Vsync GetVsync()                 const = 0;
    virtual MouseMode GetMouseMode()         const = 0;
    virtual WindowMode GetWindowMode()       const = 0;
    virtual bool IsFullscreen()              const = 0;
    virtual Error GetInitStatus()            const = 0;
    virtual Farg<Properties> GetProperties() const = 0;
};

#endif // WINDOW_H
