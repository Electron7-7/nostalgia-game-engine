#include "glfw_window.hpp"
#include "application/application.hpp"
#include "application/monitor.hpp"
#include "managers/event_manager"
#include "events/event.hpp"
#include "events/event_queue.hpp"
#include "rendering/graphics_context.hpp"
#include "core/printing.hpp"

#include <vector>
#include <glad/glad.h>
#include <glm/vec2.hpp>

static std::vector<Unique<Monitor>> m_sMonitors{};

void WindowGLFW::CallbackHandler::sMonitorCallbackFunction(GLFWmonitor* inMonitor, int inEvent)
{
    if(inEvent == GLFW_CONNECTED)
    {
        m_sMonitors.push_back(std::make_unique<Monitor>(inMonitor, glfwGetMonitorName(inMonitor)));
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
    else if(inEvent == GLFW_DISCONNECTED)
    {
        // Pun intended
        if(auto found_it{std::find(m_sMonitors.begin(), m_sMonitors.end(), Monitor{inMonitor})};
            found_it != m_sMonitors.end())
            { m_sMonitors.erase(found_it); }
    }
}

#pragma message("TODO: add an 'are you sure?' message or something similar for closing the window")
void WindowGLFW::CallbackHandler::sWindowCloseCallbackFunction(GLFWwindow*)
{ Application()->Stop(); }

#pragma message("TODO: send out a 'window resize' event (i'll need to implement the non-input event system)")
void WindowGLFW::CallbackHandler::sWindowSizeCallbackFunction(GLFWwindow* inWindow, int inWidth, int inHeight)
{
    auto pWindow{static_cast<WindowGLFW*>(glfwGetWindowUserPointer(inWindow))};
    pWindow->mData.width  = inWidth;
    pWindow->mData.height = inHeight;
}

void WindowGLFW::CallbackHandler::sWindowPosCallbackFunction(GLFWwindow* inWindow, int inX, int inY)
{
    auto pWindow{static_cast<WindowGLFW*>(glfwGetWindowUserPointer(inWindow))};
    pWindow->mData.x_pos = inX;
    pWindow->mData.y_pos = inY;
}

void WindowGLFW::CallbackHandler::sCursorPosCallbackFunction(GLFWwindow* inWindow, double inX, double inY)
{
    auto pWindow{static_cast<WindowGLFW*>(glfwGetWindowUserPointer(inWindow))};
    pWindow->mMouseLast = pWindow->mMouseCurrent;
    pWindow->mMouseCurrent.x = inX;
    pWindow->mMouseCurrent.y = inY;
}

Key::Modifiers sGetModifierKeys(int mods)
{
    Key::Modifiers out_mods{};
    if(mods & GLFW_MOD_SHIFT)
        { out_mods.mods[(ushort)Key::Modifier::MOD_SHIFT] = true; }
    if(mods & GLFW_MOD_CONTROL)
        { out_mods.mods[(ushort)Key::Modifier::MOD_CONTROL] = true; }
    if(mods & GLFW_MOD_ALT)
        { out_mods.mods[(ushort)Key::Modifier::MOD_ALT] = true; }
    if(mods & GLFW_MOD_SUPER)
        { out_mods.mods[(ushort)Key::Modifier::MOD_SUPER] = true; }
    if(mods & GLFW_MOD_CAPS_LOCK)
        { out_mods.mods[(ushort)Key::Modifier::MOD_CAPS_LOCK] = true; }
    if(mods & GLFW_MOD_NUM_LOCK)
        { out_mods.mods[(ushort)Key::Modifier::MOD_NUM_LOCK] = true; }
    return out_mods;
}

void WindowGLFW::CallbackHandler::sKeyCallbackFunction(GLFWwindow* inWindow,
    int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_UNKNOWN)
        { return; }
    else if(auto found_it{s_cGLFWInputLookup.find(key)};
        found_it != s_cGLFWInputLookup.end())
    {
        g_pEventManager->GetListeningInputEventQueue()
            ->add<InputEventBinding>(found_it->second,
                sGetModifierKeys(mods),
                action != GLFW_RELEASE,
                action == GLFW_REPEAT);
    }
}

WindowGLFW::WindowGLFW(const WindowProperties& inProperties)
{ mInitStatus = Init(inProperties); }

WindowGLFW::~WindowGLFW()
{ Shutdown(); }

Error WindowGLFW::Init(const WindowProperties& inProperties)
{
    PRINT_PRETTY_FUNCTION;
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
    print_error_enum(mGraphicsContext->Init());
    return print_error_enum(InitializeCallbacks());
}

Error WindowGLFW::InitializeCallbacks()
{
    glfwSetMonitorCallback(&CallbackHandler::sMonitorCallbackFunction);
    glfwSetWindowCloseCallback(m_pWindow, &CallbackHandler::sWindowCloseCallbackFunction);
    glfwSetWindowSizeCallback(m_pWindow, &CallbackHandler::sWindowSizeCallbackFunction);
    glfwSetWindowPosCallback(m_pWindow, &CallbackHandler::sWindowPosCallbackFunction);
    glfwSetCursorPosCallback(m_pWindow, &CallbackHandler::sCursorPosCallbackFunction);
    glfwSetKeyCallback(m_pWindow, &CallbackHandler::sKeyCallbackFunction);
    return OK;
}

void WindowGLFW::Shutdown()
{
    glfwDestroyWindow(m_pWindow);
    glfwTerminate();
}

void WindowGLFW::Update()
{
    glfwPollEvents();
    mGraphicsContext->SwapBuffers();
}

WINDOW_SET_POSITION_DEFINITION(WindowGLFW, inPosition)
{
    glfwSetWindowPos(m_pWindow, static_cast<int>(inPosition.x), static_cast<int>(inPosition.y));
    return OK;
}

WINDOW_SET_SCALE_DEFINITION(WindowGLFW, inScale)
{
    glfwSetWindowSize(m_pWindow, inScale.width, inScale.height);
    return OK;
}

Error WindowGLFW::SetVsync(Vsync)
{ return UNIMPLEMENTED; }

Error WindowGLFW::SetMouseMode(MouseMode inMode)
{
    if(inMode == mData.mouse_mode)
        { return OK; }
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
    mData.mouse_mode = inMode;
    glfwSetInputMode(m_pWindow, GLFW_CURSOR, cursor_mode);
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

Position WindowGLFW::GetMousePosition()
{ return mMouseCurrent; }

Position WindowGLFW::GetLastMousePosition()
{ return mMouseLast; }

#                                                  define DO_NOT_BUILD
#                                                  ifndef DO_NOT_BUILD

using namespace Settings;

bool WindowGLFW::Init()
{
    PRINT_PRETTY_FUNCTION;
    glfwInit();
    if(auto window_status = CreateMainWindow(); window_status != Status::NO_ERR)
    {
        glfwTerminate();
        return print_error("{}", window_status.Printout());
    }
    SetRawMouseMotion(Settings::Player::RawMouseMotion);
    mCompatibleGraphicsBackends = { BackendIDs::gOpenGL };
    return true;
}

bool WindowGLFW::InitImGui()
{
    switch((uint)g_pBackendManager->Graphics()->GetID())
    {
    case (uint)BackendIDs::gOpenGL:
        if(!ImGui_ImplGlfw_InitForOpenGL(mMainWindow, true))
            { return print_error("ImGui_ImplGlfw_InitForOpenGL returned false!"); }
        break;
    }
    return true;
}

void WindowGLFW::Shutdown()
{
    glfwDestroyWindow(mMainWindow);
    glfwTerminate();
}

void WindowGLFW::ShutdownImGui()
{ ImGui_ImplGlfw_Shutdown(); }

void WindowGLFW::ImGuiNewFrame()
{ ImGui_ImplGlfw_NewFrame(); }

#pragma message("TODO: Implement a way to dynamically clear the depth bit if 3D is enabled")
// https://gamedev.stackexchange.com/a/150215
void WindowGLFW::ClearBuffer(const glm::vec4& color)
{
    glClearColor(color.x, color.y, color.z, color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

SafeStatus WindowGLFW::CreateMainWindow()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = (Window::Fullscreen())
        ? glfwGetPrimaryMonitor()
        : nullptr;

    mMainWindow = glfwCreateWindow(Window::Size().width, Window::Size().height, Window::c_Name(), monitor, nullptr);

    if(!mMainWindow)
        { return Status::WindowingBackendWINDOW_CREATION_FAILED; }

    glfwMakeContextCurrent(mMainWindow);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        print_debug("Failed to initialize GLAD!");
        return Status::WindowingBackendGRAPHICS_INIT_FAILED;
    }
    else if(!Window::Fullscreen())
    {
        int monitor_position_x{0}, monitor_position_y{0};
        glfwGetMonitorPos(glfwGetPrimaryMonitor(), &monitor_position_x, &monitor_position_y);

        int window_position_x{((glfwGetVideoMode(glfwGetPrimaryMonitor())->width  - Window::Size().width)  / 2) + monitor_position_x};
        int window_position_y{((glfwGetVideoMode(glfwGetPrimaryMonitor())->height - Window::Size().height) / 2) + monitor_position_y};
        glfwSetWindowPos(mMainWindow, window_position_x, window_position_y);
    }

    glfwSetWindowPosCallback(mMainWindow, WindowGLFW::m_sWindowPositionCallbackFunction);
    glfwSetWindowSizeCallback(mMainWindow, WindowGLFW::m_sWindowSizeCallbackFunction);
    glfwSetFramebufferSizeCallback(mMainWindow, WindowGLFW::m_sFrameBufferSizeCallbackFunction);

    mLastFullscreenedMonitor = glfwGetPrimaryMonitor();
    return Status::NO_ERR;
}

void WindowGLFW::ToggleRawMouseMotion()
{ SetRawMouseMotion(!Settings::Player::RawMouseMotion); }

MouseMode WindowGLFW::ToggleMouseMode(MouseMode secondary, MouseMode primary)
{
    if(mMouseMode == primary)
        { SetMouseMode(secondary); }
    else
        { SetMouseMode(primary); }
    return mMouseMode;
}

void WindowGLFW::SetRawMouseMotion(bool toggle)
{
    Settings::Player::RawMouseMotion = toggle;
    if(glfwRawMouseMotionSupported() && Settings::Player::RawMouseMotion)
        { glfwSetInputMode(mMainWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE); }
}

bool WindowGLFW::SetMouseMode(MouseMode mode)
{
    int glfw_mode;
    switch(mode)
    {
    case MouseMode::Captured:
        glfw_mode = GLFW_CURSOR_CAPTURED;
        break;
    case MouseMode::Disabled:
        glfw_mode = GLFW_CURSOR_DISABLED;
        break;
    case MouseMode::Hidden:
        glfw_mode = GLFW_CURSOR_HIDDEN;
        break;
    case MouseMode::Normal:
        glfw_mode = GLFW_CURSOR_NORMAL;
        break;
    }
    mMouseMode = mode;
    glfwSetInputMode(mMainWindow, GLFW_CURSOR, glfw_mode);
    return true;
}

MouseMode WindowGLFW::GetMouseMode()
{ return mMouseMode; }

void WindowGLFW::GetMousePosition(glm::vec2& output)
{
    double l_Position[2] = {0.0, 0.0};
    glfwGetCursorPos(mMainWindow, &l_Position[0], &l_Position[1]);
    output = glm::vec2{l_Position[0], l_Position[1]};
}

bool WindowGLFW::UpdateBinding(InputBinding& binding)
{
    if(!m_sInputIdToGlfw.contains(binding.id()))
        { return false; }
    switch(glfwGetKey(mMainWindow, m_sInputIdToGlfw.at(binding.id())))
    {
    case GLFW_PRESS:
        binding.Press();
        return true;
    case GLFW_RELEASE:
        return binding.Release();
    }
    return false;
}

void WindowGLFW::SwapBuffers()
{ glfwSwapBuffers(mMainWindow); }

void WindowGLFW::PollEvents()
{ glfwPollEvents(); }

void WindowGLFW::UpdateState()
{
    glfwSetWindowTitle(mMainWindow, Window::c_Name());
    if((glfwGetWindowMonitor(mMainWindow) != nullptr) == Window::Fullscreen())
    { // Fullscreen status hasn't changed
        glfwSetWindowPos(mMainWindow, Window::Position().x, Window::Position().y);
        glfwSetWindowSize(mMainWindow, Window::Size().width, Window::Size().height);
        return;
    }
    glfwSetWindowMonitor(mMainWindow,
        (Window::Fullscreen()) ? mLastFullscreenedMonitor : nullptr,
        Window::Position().x,
        Window::Position().y,
        Window::Size().width,
        Window::Size().height,
        GLFW_DONT_CARE);
}

void WindowGLFW::m_sWindowPositionCallbackFunction(GLFWwindow* window, int x, int y)
{ s_vInfo().position = {x, y}; }

void WindowGLFW::m_sWindowSizeCallbackFunction(GLFWwindow* window, int width, int height)
{ s_vInfo().size = {width, height}; }

void WindowGLFW::m_sFrameBufferSizeCallbackFunction(GLFWwindow* window, int width, int height)
{
    s_vInfo().framebuffer_size = {width, height};
    g_pBackendManager->Graphics()->SetWindowViewport(Viewport{Window::FramebufferSize(), Window::FramebufferPosition()});
}
#endif // DO_NOT_BUILD
