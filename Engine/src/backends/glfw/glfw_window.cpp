#include "glfw_window.hpp"
#include "thirdparty/frozen/map.h"
#include <Nostalgia/application/monitor.hpp>
#include <Nostalgia/events/event.hpp>
#include <Nostalgia/events/event_queue.hpp>
#include <Nostalgia/managers/event_manager.hpp>
#include <Nostalgia/managers/input_manager.hpp>
#include <Nostalgia/managers/render_manager.hpp>
#include <Nostalgia/rendering/renderer_api.hpp>
#include <Nostalgia/rendering/graphics_context.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3native.h>

static int sShittyWindowTrackerPleaseMakeSomethingBetter{0};
static std::vector<Unique<Monitor>> m_sMonitors{};

static constinit const frozen::map<GLFWInputID, KeyID, Key::KeysCount>
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
    {GLFW_MOUSE_BUTTON_LEFT,   Key::MouseLeft},
    {GLFW_MOUSE_BUTTON_RIGHT,  Key::MouseRight},
    {GLFW_MOUSE_BUTTON_MIDDLE, Key::MouseMiddle},
};

bool gDebugPrintStateChanges{false};

void WindowGLFW::CallbackHandler::sMonitorCallbackFunction(GLFWmonitor* inMonitor, int inEvent)
{
    if(inEvent == GLFW_CONNECTED)
        { AddMonitor(inMonitor); }
    else if(inEvent == GLFW_DISCONNECTED)
    {
        // Pun intended
        if(auto found_it{std::find(m_sMonitors.begin(), m_sMonitors.end(), Monitor{inMonitor})};
            found_it != m_sMonitors.end())
                { m_sMonitors.erase(found_it); }
    }
}

void WindowGLFW::CallbackHandler::sWindowCloseCallbackFunction(GLFWwindow*)
{ EventManager::Queue()->add<AppEvent>(AppEvent::WindowClose); }

void WindowGLFW::CallbackHandler::sWindowSizeCallbackFunction(GLFWwindow* inWindow, int inWidth, int inHeight)
{
    auto pWindow{static_cast<WindowGLFW*>(glfwGetWindowUserPointer(inWindow))};
    pWindow->mData.width  = inWidth;
    pWindow->mData.height = inHeight;
    g_pRenderManager->GetAPI()->SetViewport({0, 0}, pWindow->GetScale());
    EventManager::Queue()->add<AppEvent>(AppEvent::WindowResize);
}

void WindowGLFW::CallbackHandler::sWindowPosCallbackFunction(GLFWwindow* inWindow, int inX, int inY)
{
    auto pWindow{static_cast<WindowGLFW*>(glfwGetWindowUserPointer(inWindow))};
    pWindow->mData.x_pos = inX;
    pWindow->mData.y_pos = inY;
    EventManager::Queue()->add<AppEvent>(AppEvent::WindowMoved);
}

void WindowGLFW::CallbackHandler::sCursorPosCallbackFunction(GLFWwindow* inWindow, double inX, double inY)
{ static_cast<WindowGLFW*>(glfwGetWindowUserPointer(inWindow))->mFramesWithNoMouseMovement = 0; }

Key::Modifiers sConvertModifierKeys(int mods)
{
    Key::Modifiers out_mods{};
    out_mods.set(Key::Mod_Shift,    mods & GLFW_MOD_SHIFT);
    out_mods.set(Key::Mod_Control,  mods & GLFW_MOD_CONTROL);
    out_mods.set(Key::Mod_Alt,      mods & GLFW_MOD_ALT);
    out_mods.set(Key::Mod_Super,    mods & GLFW_MOD_SUPER);
    out_mods.set(Key::Mod_CapsLock, mods & GLFW_MOD_CAPS_LOCK);
    out_mods.set(Key::Mod_NumLock,  mods & GLFW_MOD_NUM_LOCK);
    return out_mods;
}

void WindowGLFW::CallbackHandler::sKeyCallbackFunction(GLFWwindow* inWindow,
    int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_UNKNOWN)
        { return; }
    else if(const auto& found_it{s_cGLFWInputLookup.find(key)};
        found_it != s_cGLFWInputLookup.end())
    {
        g_pInputManager->Queue()->add<InputEventBinding>(found_it->second[],
                sConvertModifierKeys(mods),
                action != GLFW_RELEASE,
                action == GLFW_REPEAT,
                g_pInputManager->UpdateKeyState(found_it->second, action != GLFW_RELEASE));
    }
}

void WindowGLFW::CallbackHandler::sMouseButtonCallbackFunction(GLFWwindow* inWindow, int button, int action, int mods)
{
    if(button == GLFW_KEY_UNKNOWN)
        { return; }
    else if(const auto& found_it{s_cGLFWInputLookup.find(button)};
        found_it != s_cGLFWInputLookup.end())
    {
        g_pInputManager->Queue()->add<InputEventBinding>(found_it->second[],
                sConvertModifierKeys(mods),
                action != GLFW_RELEASE,
                action == GLFW_REPEAT,
                g_pInputManager->UpdateKeyState(found_it->second, action != GLFW_RELEASE));
    }
}

void WindowGLFW::AddMonitor(GLFWmonitor* inMonitor)
{
    m_sMonitors.push_back(MakeUnique<Monitor>(inMonitor, glfwGetMonitorName(inMonitor)));
    auto& outMonitor{m_sMonitors.back()};
    glfwGetMonitorPos(inMonitor,
        &outMonitor->virtual_position.x, &outMonitor->virtual_position.y);
    glfwGetMonitorWorkarea(inMonitor,
        &outMonitor->work_area.xpos, &outMonitor->work_area.ypos,
        &outMonitor->work_area.width, &outMonitor->work_area.height);
    glfwGetMonitorPhysicalSize(inMonitor,
        &outMonitor->physical_size.width_mm, &outMonitor->physical_size.height_mm);
    glfwGetMonitorContentScale(inMonitor,
        &outMonitor->content_scale.x, &outMonitor->content_scale.y);
}

WindowGLFW::WindowGLFW(const WindowProperties& inProperties)
{
    mInitStatus = Init(inProperties);
    EnumPrettifier::Register(MOUSE_MODE_VISIBLE,  "MOUSE_MODE_VISIBLE");
    EnumPrettifier::Register(MOUSE_MODE_CAPTURED, "MOUSE_MODE_CAPTURED");
    EnumPrettifier::Register(MOUSE_MODE_HIDDEN,   "MOUSE_MODE_HIDDEN");
    EnumPrettifier::Register(MOUSE_MODE_DISABLED, "MOUSE_MODE_DISABLED");
    PRINT_PRETTY_CONSTRUCTOR;
}

WindowGLFW::~WindowGLFW()
{
    Shutdown();
    PRINT_PRETTY_DESTRUCTOR;
}

Error WindowGLFW::Init(const WindowProperties& inProperties)
{
    mData = inProperties;
    if(!glfwInit())
        { return print_error_enum(ERR_INIT_FAILED); }
    m_pWindow = glfwCreateWindow(
        static_cast<int>(mData.width),
        static_cast<int>(mData.height),
        mData.title.data(),
        nullptr, nullptr);
    mGraphicsContext = IGraphicsContext::CreateContext(m_pWindow);
    glfwSetWindowUserPointer(m_pWindow, this);
    if(!print_error_enum(mGraphicsContext->Init()))
        { glfwTerminate(); return ERR_INIT_FAILED; }
    print_error_enum(InitializeCallbacks());
    ++sShittyWindowTrackerPleaseMakeSomethingBetter;

    if(m_sMonitors.empty())
    {
        int count{};
        auto monitors{glfwGetMonitors(&count)};
        for(int i{0}; i < count; ++i)
            { AddMonitor(monitors[i]); }
        mFullscreenMonitorIndex = 0;
    }
    return OK;
}

Error WindowGLFW::InitializeCallbacks()
{
    glfwSetMonitorCallback(&CallbackHandler::sMonitorCallbackFunction);
    glfwSetWindowCloseCallback(m_pWindow, &CallbackHandler::sWindowCloseCallbackFunction);
    glfwSetWindowSizeCallback(m_pWindow, &CallbackHandler::sWindowSizeCallbackFunction);
    glfwSetWindowPosCallback(m_pWindow, &CallbackHandler::sWindowPosCallbackFunction);
    glfwSetCursorPosCallback(m_pWindow, &CallbackHandler::sCursorPosCallbackFunction);
    glfwSetKeyCallback(m_pWindow, &CallbackHandler::sKeyCallbackFunction);
    glfwSetMouseButtonCallback(m_pWindow, &CallbackHandler::sMouseButtonCallbackFunction);
    return OK;
}

void WindowGLFW::Shutdown()
{
    glfwDestroyWindow(m_pWindow);
    if(--sShittyWindowTrackerPleaseMakeSomethingBetter <= 0)
        { glfwTerminate(); }
}

void WindowGLFW::Update()
{
    mGraphicsContext->SwapBuffers();
    glfwPollEvents();
    mMouseLast = mMouseCurrent;
    glfwGetCursorPos(m_pWindow, &mMouseCurrent[0], &mMouseCurrent[1]);
    if(mMouseCurrent != mMouseLast or ++mFramesWithNoMouseMovement < mFrameLimitForNoMouseMovement)
        { g_pInputManager->Queue()->add<InputEventMouseMotion>(mMouseCurrent, mMouseLast); }
}

WINDOW_SET_POSITION_DEFINITION(WindowGLFW, inPosition)
{
    glfwSetWindowPos(m_pWindow, inPosition.x(), inPosition.y());
    return OK;
}

WINDOW_SET_SCALE_DEFINITION(WindowGLFW, inScale)
{
    glfwSetWindowSize(m_pWindow, inScale.width(), inScale.height());
    return OK;
}

Error WindowGLFW::SetVsync(Vsync)
{ return UNIMPLEMENTED; }

Error WindowGLFW::SetMouseMode(MouseMode inMode)
{
    if(inMode == mData.mouse_mode)
    {
        if(gDebugPrintStateChanges)
            { print_debug("Mouse mode set to current mouse mode (no changes made)"); }
        return OK;
    }
    int cursor_mode{-1};
    switch(inMode)
    {
    case MOUSE_MODE_VISIBLE:
        cursor_mode = GLFW_CURSOR_NORMAL;
        break;
    case MOUSE_MODE_CAPTURED:
        cursor_mode = GLFW_CURSOR_CAPTURED;
        break;
    case MOUSE_MODE_HIDDEN:
        cursor_mode = GLFW_CURSOR_HIDDEN;
        break;
    case MOUSE_MODE_DISABLED:
        cursor_mode = GLFW_CURSOR_DISABLED;
        break;
    default:
        return ERR_SWITCH_DEFAULT;
    }
    glfwSetInputMode(m_pWindow, GLFW_CURSOR, cursor_mode);
    if(gDebugPrintStateChanges)
    {
        print_debug("Mouse mode set: {} (from: {})",
            EnumPrettifier::Prettify(inMode),
            EnumPrettifier::Prettify(mData.mouse_mode));
    }
    mData.mouse_mode = inMode;
    return OK;
}

Error WindowGLFW::SetWindowMode(WindowMode inMode)
{
    if(mData.window_mode == inMode)
        { return OK; }
    GLFWmonitor* monitor{nullptr};
    switch(inMode)
    {
    case WINDOW_MODE_WINDOWED:
        break;
    case WINDOW_MODE_FULLSCREEN:
        monitor = static_cast<GLFWmonitor*>(GetFullscreenMonitor()->native_monitor);
        break;
    case WINDOW_MODE_BORDERLESS:
        // glfwSetWindowMonitor(...) -> as above, but with saved/queried monitor properties
        return UNIMPLEMENTED;
    default:
        return ERR_SWITCH_DEFAULT;
    }
    mData.window_mode = inMode;
    glfwSetWindowMonitor(m_pWindow, monitor, mData.x_pos, mData.y_pos, mData.width, mData.height, GLFW_DONT_CARE);
    return OK;
}

const Unique<Monitor>& WindowGLFW::GetPrimaryMonitor() const
{
    // Pun intended
    auto found_it{std::find(m_sMonitors.begin(), m_sMonitors.end(), Monitor{glfwGetPrimaryMonitor()})};
    if(found_it == m_sMonitors.end())
        { found_it = m_sMonitors.begin(); }
    return *found_it;
}

uint WindowGLFW::GetFullscreenMonitorIndex()
{
    assert(m_sMonitors.size() > 0 && "WindowGLFW::m_sMonitors.size() <= 0");
    if(mFullscreenMonitorIndex >= m_sMonitors.size())
        { mFullscreenMonitorIndex = 0; }
    return mFullscreenMonitorIndex;
}

const Unique<Monitor>& WindowGLFW::GetFullscreenMonitor()
{ return m_sMonitors.at(GetFullscreenMonitorIndex()); }

Error WindowGLFW::SetFullscreenMonitor(uint MonitorIndex)
{
    if(MonitorIndex < m_sMonitors.size())
    {
        mFullscreenMonitorIndex = MonitorIndex;
        if(IsFullscreen())
        {
            mData.window_mode = WINDOW_MODE_WINDOWED;
            SetWindowMode(WINDOW_MODE_FULLSCREEN);
        }
        return OK;
    }
    return ERR_INDEX_OUT_OF_BOUNDS;
}

Position2D WindowGLFW::GetMousePosition()
{ return mMouseCurrent; }

Position2D WindowGLFW::GetLastMousePosition()
{ return mMouseLast; }

Unique<IGraphicsContext>& WindowGLFW::GetContext()
{ return mGraphicsContext; }
