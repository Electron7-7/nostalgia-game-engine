#ifndef GLFW_BACKEND_H
#define GLFW_BACKEND_H

#include <Nostalgia/events/bindings.hpp>
#include <Nostalgia/application/window.hpp>
#include <Nostalgia/thirdparty/glad/glad.h>
#include <Nostalgia/thirdparty/glfw/include/GLFW/glfw3.h>
#include <Nostalgia/thirdparty/frozen/map.h>

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
    Unique<GraphicsContext>& GetContext() override;

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
        static void sMouseButtonCallbackFunction(GLFWwindow*,int,int,int);
        static void sMouseScrollCallbackFunction(GLFWwindow*,double,double);

        inline static WindowGLFW* s_pWindow{nullptr};
    };
    friend struct CallbackHandler;

    GLFWwindow* m_pWindow{nullptr};
    Unique<GraphicsContext> mGraphicsContext{nullptr};
    Position2D mMouseCurrent{};
    Position2D mMouseLast{};
    Position2D mScrollCurrent{};
    int mFramesWithNoMouseMovement{0}, mFramesWithNoScrollOffset{0};

    Error InitializeCallbacks() override;
    Error Init(const WindowProperties&);
    void Shutdown();

    static constexpr int mFrameLimitForNoMouseMovement{2};
    static uint GetMonitorIndex(GLFWmonitor*);
    static void AddMonitor(GLFWmonitor*);

    inline static constinit const frozen::map<GLFWInputID, KeyID, Key::KeysCount>
    s_cGLFWInputLookup
    {
        {GLFW_KEY_0, Key::Zero},
        {GLFW_KEY_1, Key::One},
        {GLFW_KEY_2, Key::Two},
        {GLFW_KEY_3, Key::Three},
        {GLFW_KEY_4, Key::Four},
        {GLFW_KEY_5, Key::Five},
        {GLFW_KEY_6, Key::Six},
        {GLFW_KEY_7, Key::Seven},
        {GLFW_KEY_8, Key::Eight},
        {GLFW_KEY_9, Key::Nine},
        {GLFW_KEY_A, Key::A},
        {GLFW_KEY_B, Key::B},
        {GLFW_KEY_C, Key::C},
        {GLFW_KEY_D, Key::D},
        {GLFW_KEY_E, Key::E},
        {GLFW_KEY_F, Key::F},
        {GLFW_KEY_G, Key::G},
        {GLFW_KEY_H, Key::H},
        {GLFW_KEY_I, Key::I},
        {GLFW_KEY_J, Key::J},
        {GLFW_KEY_K, Key::K},
        {GLFW_KEY_L, Key::L},
        {GLFW_KEY_M, Key::M},
        {GLFW_KEY_N, Key::N},
        {GLFW_KEY_O, Key::O},
        {GLFW_KEY_P, Key::P},
        {GLFW_KEY_Q, Key::Q},
        {GLFW_KEY_R, Key::R},
        {GLFW_KEY_S, Key::S},
        {GLFW_KEY_T, Key::T},
        {GLFW_KEY_U, Key::U},
        {GLFW_KEY_V, Key::V},
        {GLFW_KEY_W, Key::W},
        {GLFW_KEY_X, Key::X},
        {GLFW_KEY_Y, Key::Y},
        {GLFW_KEY_Z, Key::Z},
        {GLFW_KEY_F1,  Key::F1},
        {GLFW_KEY_F2,  Key::F2},
        {GLFW_KEY_F3,  Key::F3},
        {GLFW_KEY_F4,  Key::F4},
        {GLFW_KEY_F5,  Key::F5},
        {GLFW_KEY_F6,  Key::F6},
        {GLFW_KEY_F7,  Key::F7},
        {GLFW_KEY_F8,  Key::F8},
        {GLFW_KEY_F9,  Key::F9},
        {GLFW_KEY_F10, Key::F10},
        {GLFW_KEY_F11, Key::F11},
        {GLFW_KEY_F12, Key::F12},
        {GLFW_KEY_LEFT_SHIFT,    Key::LeftShift},
        {GLFW_KEY_RIGHT_SHIFT,   Key::RightShift},
        {GLFW_KEY_LEFT_CONTROL,  Key::LeftControl},
        {GLFW_KEY_RIGHT_CONTROL, Key::RightControl},
        {GLFW_KEY_LEFT_ALT,      Key::LeftAlt},
        {GLFW_KEY_RIGHT_ALT,     Key::RightAlt},
        {GLFW_KEY_LEFT_SUPER,    Key::LeftSuper},
        {GLFW_KEY_RIGHT_SUPER,   Key::RightSuper},
        {GLFW_KEY_ENTER,     Key::Enter},
        {GLFW_KEY_DELETE,    Key::Delete},
        {GLFW_KEY_BACKSPACE, Key::Backspace},
        {GLFW_KEY_TAB,       Key::Tab},
        {GLFW_KEY_SPACE,     Key::Space},
        {GLFW_KEY_UP,        Key::Up},
        {GLFW_KEY_DOWN,      Key::Down},
        {GLFW_KEY_LEFT,      Key::Left},
        {GLFW_KEY_RIGHT,     Key::Right},
        {GLFW_KEY_HOME,      Key::Home},
        {GLFW_KEY_END,       Key::End},
        {GLFW_KEY_PAGE_UP,   Key::PgUp},
        {GLFW_KEY_PAGE_DOWN, Key::PgDn},
        {GLFW_KEY_ESCAPE, Key::Escape},
        {GLFW_KEY_GRAVE_ACCENT, Key::Tilde},
        {GLFW_MOUSE_BUTTON_LEFT,   Key::MouseLeft},
        {GLFW_MOUSE_BUTTON_RIGHT,  Key::MouseRight},
        {GLFW_MOUSE_BUTTON_MIDDLE, Key::MouseMiddle},
    };
};

extern bool gDebugPrintStateChanges;

#endif // GLFW_BACKEND_H
