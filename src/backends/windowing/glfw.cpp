#include "glfw.hpp"
#include "common/printing.hpp"
#include "math/glm_format.hpp" // IWYU pragma: keep
#include "managers/backend_manager.hpp"
#include "input/binding.hpp"
#include "settings/window.hpp"
#include "settings/player.hpp"
#include "thirdparty/DearImGui/imgui_impl_glfw.h"
#include "thirdparty/GLFW/glfw3.h"
#include "thirdparty/glad/glad.h"

#include <glm/vec2.hpp>

using namespace Settings;

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

#pragma message("TODO: Implement a way to dynamically clear the depth bit if 3D is enabled")
// https://gamedev.stackexchange.com/a/150215
void GLFW_Backend::ClearBuffer(const glm::vec4& color)
{
    glClearColor(color.x, color.y, color.z, color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

SafeStatus GLFW_Backend::CreateMainWindow()
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

    glfwSetWindowPosCallback(mMainWindow, GLFW_Backend::m_sWindowPositionCallbackFunction);
    glfwSetWindowSizeCallback(mMainWindow, GLFW_Backend::m_sWindowSizeCallbackFunction);
    glfwSetFramebufferSizeCallback(mMainWindow, GLFW_Backend::m_sFrameBufferSizeCallbackFunction);

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
    glfwSetWindowTitle(mMainWindow, Window::c_Name());
    if((glfwGetWindowMonitor(mMainWindow) != nullptr) == Window::Fullscreen())
    { // Fullscreen status hasn't changed
        glfwSetWindowPos(mMainWindow, Window::Position().x, Window::Position().y);
        glfwSetWindowSize(mMainWindow, Window::Size().width, Window::Size().height);
        glViewport(0, 0, Window::FramebufferSize().width, Window::FramebufferSize().height);
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

void GLFW_Backend::m_sWindowPositionCallbackFunction(GLFWwindow* window, int x, int y)
{ s_vInfo().position = {x, y}; }

void GLFW_Backend::m_sWindowSizeCallbackFunction(GLFWwindow* window, int width, int height)
{ s_vInfo().size = {width, height}; }

void GLFW_Backend::m_sFrameBufferSizeCallbackFunction(GLFWwindow* window, int width, int height)
{ s_vInfo().framebuffer_size = {width, height}; glViewport(0, 0, width, height); }
