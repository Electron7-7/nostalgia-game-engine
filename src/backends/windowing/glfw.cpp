#include "glfw.hpp"
#include "common/printing.hpp"
#include "math/glm_format.hpp" // IWYU pragma: keep
#include "managers/backend_manager.hpp"
#include "input/binding.hpp"
#include "settings/settings.hpp"
#include "thirdparty/DearImGui/imgui_impl_glfw.h"
#include "thirdparty/GLFW/glfw3.h"
#include "thirdparty/glad/glad.h"

#include <glm/vec2.hpp>

using namespace Settings;

// GLFW callback functions
static void s_WindowPositionCallbackFunction(GLFWwindow*, int, int);
static void s_WindowSizeCallbackFunction(GLFWwindow*, int, int);
static void s_FrameBufferSizeCallbackFunction(GLFWwindow*, int, int);

bool GLFW_Backend::Init()
{
    print_debug("GLFW_Backend::Init");
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

bool GLFW_Backend::InitImGui()
{
    switch(g_pBackendManager->Graphics()->GetID())
    {
    case BackendIDs::gOpenGL:
        if(!ImGui_ImplGlfw_InitForOpenGL(mMainWindow, true))
            { return print_error("GLFW_Backend::InitImGui - ImGui_ImplGlfw_InitForOpenGL returned false!"); }
        break;
    }
    return true;
}

void GLFW_Backend::Shutdown()
{
    glfwDestroyWindow(mMainWindow);
    glfwTerminate();
}

void GLFW_Backend::ShutdownImGui()
{ ImGui_ImplGlfw_Shutdown(); }

void GLFW_Backend::ImGuiNewFrame()
{ ImGui_ImplGlfw_NewFrame(); }

SafeStatus GLFW_Backend::CreateMainWindow()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* is_fullscreen = (Window::Fullscreen())
        ? nullptr
        : glfwGetPrimaryMonitor();

    mMainWindow = glfwCreateWindow(Window::Size().width, Window::Size().height, Window::c_Name(), is_fullscreen, nullptr);

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

    glfwSetWindowPosCallback(mMainWindow, s_WindowPositionCallbackFunction);
    glfwSetWindowSizeCallback(mMainWindow, s_WindowSizeCallbackFunction);
    glfwSetFramebufferSizeCallback(mMainWindow, s_FrameBufferSizeCallbackFunction);

    mLastFullscreenedMonitor = glfwGetPrimaryMonitor();
    return Status::NO_ERR;
}

void GLFW_Backend::ToggleRawMouseMotion()
{ SetRawMouseMotion(!Settings::Player::RawMouseMotion); }

MouseMode GLFW_Backend::ToggleMouseMode(MouseMode secondary, MouseMode primary)
{
    if(mMouseMode == primary)
        { SetMouseMode(secondary); }
    else
        { SetMouseMode(primary); }
    return mMouseMode;
}

void GLFW_Backend::SetRawMouseMotion(bool toggle)
{
    Settings::Player::RawMouseMotion = toggle;
    if(glfwRawMouseMotionSupported() && Settings::Player::RawMouseMotion)
        { glfwSetInputMode(mMainWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE); }
}

bool GLFW_Backend::SetMouseMode(MouseMode mode)
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

MouseMode GLFW_Backend::GetMouseMode()
{ return mMouseMode; }

void GLFW_Backend::GetMousePosition(glm::vec2& output)
{
    double l_Position[2] = {0.0, 0.0};
    glfwGetCursorPos(mMainWindow, &l_Position[0], &l_Position[1]);
    output = glm::vec2{l_Position[0], l_Position[1]};
}

bool GLFW_Backend::UpdateBinding(InputBinding& binding)
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

void GLFW_Backend::SwapBuffers()
{ glfwSwapBuffers(mMainWindow); }

void GLFW_Backend::PollEvents()
{ glfwPollEvents(); }

void GLFW_Backend::UpdateState()
{
    // if fullscreen status hasn't changed...
    if(static_cast<bool>(glfwGetWindowMonitor(mMainWindow)) == Window::Fullscreen())
    {
        glfwSetWindowPos(mMainWindow, Window::Position().x, Window::Position().y);
        glfwSetWindowSize(mMainWindow, Window::Size().width, Window::Size().height);
        glfwSetWindowTitle(mMainWindow, Window::c_Name());
        return;
    }
    auto monitor = (Window::Fullscreen()) ? mLastFullscreenedMonitor : nullptr;
    glfwSetWindowMonitor(mMainWindow,
        monitor,
        Window::Position().x,
        Window::Position().y,
        Window::Size().width,
        Window::Size().height,
        GLFW_DONT_CARE);
}

void s_WindowPositionCallbackFunction(GLFWwindow* window, int x, int y)
{ Window::setPosition({x, y}); }

void s_WindowSizeCallbackFunction(GLFWwindow* window, int width, int height)
{ Window::setSize({width, height}); }

void s_FrameBufferSizeCallbackFunction(GLFWwindow* window, int width, int height)
{ Window::setFramebufferSize({width, height}); }
