#ifndef GLFW_BACKEND_H
#define GLFW_BACKEND_H

#include "rendering/fwd.hpp"

#include "application/window.hpp"
#include "events/bindings.hpp"
#include "math/containers.hpp"
#include "frozen/map.h"

#include <GLFW/glfw3.h>

typedef int GLFWInputID;

class WindowGLFW : public IWindow
{
public:
    WindowGLFW(const WindowProperties&);
    virtual ~WindowGLFW();

    void Update() override;

    void* GetNativeWindow() const override { return static_cast<GLFWwindow*>(m_pWindow); }
    NativeWindowType GetNativeWindowType() const override { return NATIVE_GLFW_WINDOW; }

    const Unique<Monitor>& GetPrimaryMonitor() const override;
    uint GetFullscreenMonitorIndex() override;
    const Unique<Monitor>& GetFullscreenMonitor() override;
    Error SetFullscreenMonitor(uint) override;
    Position2D GetMousePosition() override;
    Position2D GetLastMousePosition() override;

    WINDOW_SET_POSITION_DECLARATION
    WINDOW_SET_SCALE_DECLARATION

    Error SetVsync(Vsync) override;
    Error SetMouseMode(MouseMode) override;
    Error SetWindowMode(WindowMode) override;

private:
    struct CallbackHandler
    {
        static void sMonitorCallbackFunction(GLFWmonitor*,int);
        static void sWindowCloseCallbackFunction(GLFWwindow*);
        static void sWindowSizeCallbackFunction(GLFWwindow*,int,int);
        static void sWindowPosCallbackFunction(GLFWwindow*,int,int);
        static void sCursorPosCallbackFunction(GLFWwindow*,double,double);
        static void sKeyCallbackFunction(GLFWwindow*,int,int,int,int);

        inline static WindowGLFW* s_pWindow{nullptr};
    };
    friend struct CallbackHandler;

    GLFWwindow* m_pWindow{nullptr};
    Unique<IGraphicsContext> mGraphicsContext{nullptr};
    Position2D mMouseCurrent{};
    Position2D mMouseLast{};

    Error InitializeCallbacks() override;
    Error Init(const WindowProperties&);
    void Shutdown();

    static constinit frozen::map<GLFWInputID, id_t, Input::KeysCount> s_cGLFWInputLookup;

    static uint GetMonitorIndex(GLFWmonitor*);
};

#endif // GLFW_BACKEND_H
