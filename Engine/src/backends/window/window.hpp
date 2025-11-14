#ifndef WINDOW_H
#define WINDOW_H

#include "common/sanity.hpp"
#include "math/containers.hpp"
#include "core/game_loop.hpp"

#include <string>
#include <memory>

enum NativeWindowType
{
    NATIVE_GLFW_WINDOW,
};

// When implementing `IWindow`, `T` is replaced with that implementation's class (e.g: `class WindowImpl : public IWindow<WindowImpl>`)
template<typename T>
class IWindow : public Updated
{
public:
    enum VSYNC_STATUS
    {
        VSYNC_OFF,      // le chud: "Nooooo! You must endure input lag to avoid screen tearing!" le gigachad: "The screen now looks like cyberpunk hacking"
        VSYNC_ON,       // Not implemented
        VSYNC_ADAPTIVE, // Not implemented
    };

    struct Properties
    {
        std::string title{""};
        scale_t scale{};
        position_t position{};
        VSYNC_STATUS vsync{VSYNC_OFF};

        constexpr bool operator==(const Properties& other) const
        {
            return title == other.title &&
                   position == other.position &&
                   scale == other.scale;
        }

        Properties(StringArg inTitle = {"Nostalgia Engine"}, ScaleArg inScale = {1280,720}, PositionArg inPosition = {0,0}):
            title{inTitle},
            scale{inScale},
            position{inPosition},
            vsync{VSYNC_OFF} {}
    };
    typedef IWindow::Properties WindowProperties;

private:
    Properties mWindowProperties{};

public:
    virtual ~IWindow() = default;

    virtual void Update() = 0;

    virtual void* GetNativeWindow() const = 0;
    virtual NativeWindowType GetNativeWindowType() const = 0; // Probably not necessary

    uint32_t GetWidth()  const { return GetProperties().scale.width;  }
    uint32_t GetHeight() const { return GetProperties().scale.height; }
    const WindowProperties& GetProperties() const { return mWindowProperties; }

    static std::unique_ptr<IWindow> CreateWindow(const WindowProperties& inProperties = {})
    {  }

#pragma message("TODO: SetEventCallback")
};

typedef IWindow::Properties WindowProperties;
#endif // WINDOW_H
