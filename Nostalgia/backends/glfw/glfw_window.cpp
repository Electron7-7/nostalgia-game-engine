#include "glfw_window.hpp"
#include "events/event.hpp"
#include "events/event_queue.hpp"
#include "managers/event_manager.hpp"
#include "managers/input_manager.hpp"
#include "rendering/renderer_api.hpp"
#include "rendering/graphics_context.hpp"

static int sShittyWindowTrackerPleaseMakeSomethingBetter{0};
static std::vector<Unique<Monitor>> m_sMonitors{};

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
    RendererAPI::Get()->SetViewport({0, 0}, pWindow->GetScale());
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
    else if(FAUTO found_it{s_cGLFWInputLookup.find(key)}; found_it != s_cGLFWInputLookup.end())
    {
        g_pInputManager->Queue()->add<InputEventBinding>(found_it->second(),
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
        g_pInputManager->Queue()->add<InputEventBinding>(found_it->second(),
                sConvertModifierKeys(mods),
                action != GLFW_RELEASE,
                action == GLFW_REPEAT,
                g_pInputManager->UpdateKeyState(found_it->second, action != GLFW_RELEASE));
    }
}

void WindowGLFW::CallbackHandler::sMouseScrollCallbackFunction(GLFWwindow* inWindow,
    double inOffsetX,
    double inOffsetY)
{
    static_cast<WindowGLFW*>(glfwGetWindowUserPointer(inWindow))->mFramesWithNoScrollOffset = 0;
    static_cast<WindowGLFW*>(glfwGetWindowUserPointer(inWindow))->mScrollCurrent = {inOffsetX, inOffsetY};
    g_pInputManager->UpdateScrollOffset(inOffsetX, inOffsetY);
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
    EnumRegistry::Register(MOUSE_MODE_VISIBLE,  "MOUSE_MODE_VISIBLE");
    EnumRegistry::Register(MOUSE_MODE_CAPTURED, "MOUSE_MODE_CAPTURED");
    EnumRegistry::Register(MOUSE_MODE_HIDDEN,   "MOUSE_MODE_HIDDEN");
    EnumRegistry::Register(MOUSE_MODE_DISABLED, "MOUSE_MODE_DISABLED");
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
    if(not glfwInit())
        { return print_error_enum(ERR_INIT_FAILED); }
    m_pWindow = glfwCreateWindow(
        static_cast<int>(mData.width),
        static_cast<int>(mData.height),
        mData.title.data(),
        nullptr, nullptr);
    mGraphicsContext = GraphicsContext::CreateContext(m_pWindow);
    glfwSetWindowUserPointer(m_pWindow, this);
    if(!print_error_enum(mGraphicsContext->Init()))
    {
        glfwTerminate();
        return ERR_INIT_FAILED;
    }
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
    glfwSetScrollCallback(m_pWindow, &CallbackHandler::sMouseScrollCallbackFunction);
    return OK;
}

void WindowGLFW::Shutdown()
{
    glfwDestroyWindow(m_pWindow);
    if(--sShittyWindowTrackerPleaseMakeSomethingBetter <= 0)
    {
        RendererAPI::Get()->Shutdown();
        glfwTerminate();
    }
}

void WindowGLFW::Update()
{
    glfwSwapBuffers(m_pWindow);
    glfwPollEvents();
    mMouseLast = mMouseCurrent;
    glfwGetCursorPos(m_pWindow, &mMouseCurrent[0], &mMouseCurrent[1]);
    if(mMouseCurrent != mMouseLast or ++mFramesWithNoMouseMovement < mFrameLimitForNoMouseMovement)
        { g_pInputManager->Queue()->add<InputEventMouseMotion>(mMouseCurrent, mMouseLast); }
    if(mScrollCurrent != Position2D{0.0, 0.0} and ++mFramesWithNoScrollOffset >= mFrameLimitForNoMouseMovement)
    {
        mScrollCurrent = {0.0, 0.0};
        g_pInputManager->UpdateScrollOffset(0.0, 0.0);
        mFramesWithNoScrollOffset = 0;
    }
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
            EnumRegistry::GetEnumName(inMode),
            EnumRegistry::GetEnumName(mData.mouse_mode));
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

Unique<GraphicsContext>& WindowGLFW::GetContext()
{ return mGraphicsContext; }
