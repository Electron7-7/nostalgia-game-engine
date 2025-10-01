#include "glfw.hpp"
#include "debug.hpp"
#include "managers/backend_manager.hpp"
#include "input/binding.hpp"
#include "settings/settings.hpp"
#include "DearImGui/imgui_impl_glfw.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include <glm/vec2.hpp>

typedef Settings::Window Window;

bool GLFW_Backend::Init()
{
    if(m_IsInitialized)
        { return true; }

    PRINT_DEBUG("GLFW_Backend::Init")

    glfwInit();

    SafeStatus window_status = CreateMainWindow();
    if(window_status != Status::NO_ERR)
    {
        PRINT_ERROR("{}", window_status.Printout())
        glfwTerminate();
        return false;
    }

    SetRawMouseMotion(Settings::Player::RawMouseMotion);

    m_CompatibleGraphicsBackends = { gBackendIDs::gOpenGL };
    m_IsInitialized = true;
    return true;
}

bool GLFW_Backend::InitImGui()
{
    if(m_IsImGuiInitialized)
        { return true; }

    switch(g_pBackendManager->Graphics()->GetID())
    {
    case gBackendIDs::gOpenGL:
        if(!ImGui_ImplGlfw_InitForOpenGL(m_MainWindow, true))
        {
            PRINT_ERROR("GLFW_Backend::InitImGui - ImGui_ImplGlfw_InitForOpenGL returned false!")
            return false;
        }
        break;
    }

    m_IsImGuiInitialized = true;
    return true;
}

void GLFW_Backend::Shutdown()
{
    assert(m_IsInitialized);

    if(m_IsImGuiInitialized)
        { ImGui_ImplGlfw_Shutdown(); }

    glfwDestroyWindow(m_MainWindow);
    glfwTerminate();

    m_IsInitialized = false;
    m_IsImGuiInitialized = false;
}

void GLFW_Backend::ImGuiNewFrame()
{ ImGui_ImplGlfw_NewFrame(); }

SafeStatus GLFW_Backend::CreateMainWindow()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int window_width = Window::Width;
    int window_height = Window::Height;
    GLFWmonitor* is_fullscreen = nullptr;

    if(Window::Fullscreen)
    {
        window_width = Window::FullscreenWidth;
        window_height = Window::FullscreenHeight;
        is_fullscreen = glfwGetPrimaryMonitor();
    }

    m_MainWindow = glfwCreateWindow(window_width, window_height, Window::Name, is_fullscreen, nullptr);

    if(m_MainWindow == nullptr)
        { return Status::WindowingBackendWINDOW_CREATION_FAILED; }

    glfwMakeContextCurrent(m_MainWindow);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        PRINT_DEBUG("Failed to initialize GLAD!")
        return Status::WindowingBackendGRAPHICS_INIT_FAILED;
    }

    if(!Window::Fullscreen)
    {
        int monitor_PositionX = 0;
        int monitor_PositionY = 0;
        glfwGetMonitorPos(glfwGetPrimaryMonitor(), &monitor_PositionX, &monitor_PositionY);

        int window_position_x = (((glfwGetVideoMode(glfwGetPrimaryMonitor())->width - Window::Width) / 2) + monitor_PositionX);
        int window_position_y = (((glfwGetVideoMode(glfwGetPrimaryMonitor())->height - Window::Height) / 2) + monitor_PositionY);
        glfwSetWindowPos(m_MainWindow, window_position_x, window_position_y);
    }

    m_LastFullscreenedMonitor = glfwGetPrimaryMonitor();

    glfwSetWindowSizeCallback(m_MainWindow, GLFW_Backend::glfw_WindowResizeCallbackFunction);
    glfwSetWindowPosCallback(m_MainWindow, GLFW_Backend::glfw_WindowPositionCallbackFunction);

    return Status::NO_ERR;
}

void GLFW_Backend::ResizeWindow(int width, int height)
{
    int cur_width;
    int cur_height;
    glfwGetWindowSize(m_MainWindow, &cur_width, &cur_height);
    if(width == cur_width && height == cur_height)
        { return; }
    glfwSetWindowSize(m_MainWindow, width, height);
    glfwGetWindowSize(m_MainWindow, &cur_width, &cur_height);
    if(Window::Fullscreen)
    {
        Window::FullscreenWidth = cur_width;
        Window::FullscreenHeight = cur_height;
    }
}

void GLFW_Backend::MoveWindow(int position_x, int position_y)
{
    int cur_x_pos;
    int cur_y_pos;
    glfwGetWindowPos(m_MainWindow, &cur_x_pos, &cur_y_pos);

    if(position_x == cur_x_pos && position_y == cur_y_pos)
        { return; }

    glfwSetWindowPos(m_MainWindow, position_x, position_y);
}

void GLFW_Backend::ToggleFullscreen()
{
    assert(m_IsInitialized);
    SetFullscreen(!Window::Fullscreen);
}

void GLFW_Backend::ToggleRawMouseMotion()
{ SetRawMouseMotion(!Settings::Player::RawMouseMotion); }

MouseMode GLFW_Backend::ToggleMouseMode(MouseMode secondary, MouseMode primary)
{
    if(m_MouseMode == primary)
        { SetMouseMode(secondary); }
    else
        { SetMouseMode(primary); }
    return m_MouseMode;
}

void GLFW_Backend::SetFullscreen(bool is_fullscreen_enabled)
{
    assert(m_IsInitialized);

    Window::Fullscreen = is_fullscreen_enabled;

    if(static_cast<bool>(glfwGetWindowMonitor(m_MainWindow)) == Window::Fullscreen)
        { return; }

    if(Window::Fullscreen)
        { glfwSetWindowMonitor(m_MainWindow, m_LastFullscreenedMonitor, Window::FullscreenXPosition, Window::FullscreenYPosition, Window::FullscreenWidth, Window::FullscreenHeight, GLFW_DONT_CARE); }
    else
        { glfwSetWindowMonitor(m_MainWindow, nullptr, Window::XPosition, Window::YPosition, Window::Width, Window::Height, GLFW_DONT_CARE); }
}

void GLFW_Backend::SetRawMouseMotion(bool toggle)
{
    Settings::Player::RawMouseMotion = toggle;
    if(glfwRawMouseMotionSupported() && Settings::Player::RawMouseMotion)
        { glfwSetInputMode(m_MainWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE); }
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
    m_MouseMode = mode;
    glfwSetInputMode(m_MainWindow, GLFW_CURSOR, glfw_mode);
    return true;
}

MouseMode GLFW_Backend::GetMouseMode()
{ return m_MouseMode; }

void GLFW_Backend::GetMousePosition(glm::vec2& output)
{
    double l_Position[2] = {0.0, 0.0};
    glfwGetCursorPos(m_MainWindow, &l_Position[0], &l_Position[1]);
    output = glm::vec2{l_Position[0], l_Position[1]};
}

bool GLFW_Backend::GetKey(binding_t& binding)
{
    if(!s_cInputIdToGlfw.contains(binding))
        { return false; }
    InputStatus l_LastStatus = binding.Status;
    binding.Status = static_cast<InputStatus>(glfwGetKey(m_MainWindow, s_cInputIdToGlfw.at(binding)));
    binding.JustChanged = (binding.Status != l_LastStatus);
    return (binding.JustChanged || binding.Status == InputStatus::Active);
}

bool GLFW_Backend::GetMotion(binding_t& binding, const glm::vec2& motion)
{
    if(binding != InputID::MouseMotionX && binding != InputID::MouseMotionY)
        { return false; }
    InputStatus l_LastStatus = binding.Status;
    binding.Status = static_cast<InputStatus>(
        (binding == InputID::MouseMotionX && motion.x != 0.0f) ||
        (binding == InputID::MouseMotionY && motion.y != 0.0f));
    binding.JustChanged = (binding.Status != l_LastStatus);
    return (binding.JustChanged || binding.Status == InputStatus::Active);
}

void GLFW_Backend::SwapBuffers()
{ glfwSwapBuffers(m_MainWindow); }

void GLFW_Backend::PollEvents()
{ glfwPollEvents(); }

void GLFW_Backend::UpdateState()
{
    SetFullscreen(Window::Fullscreen);
    int new_width = Window::Width;
    int new_height = Window::Height;
    int new_xpos = Window::XPosition;
    int new_ypos = Window::YPosition;

    if(Window::Fullscreen)
    {
        new_width  = Window::FullscreenWidth;
        new_height = Window::FullscreenHeight;
        new_xpos = Window::FullscreenXPosition;
        new_ypos = Window::FullscreenYPosition;
    }

    ResizeWindow(new_width, new_height);
    MoveWindow(new_xpos, new_ypos);
}

void GLFW_Backend::glfw_WindowPositionCallbackFunction(GLFWwindow* window, int position_x, int position_y)
{
    if(!Window::Fullscreen)
    {
        Window::XPosition = position_x;
        Window::YPosition = position_y;
    }
}

void GLFW_Backend::glfw_WindowResizeCallbackFunction(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    if(Window::Fullscreen)
    {
        Window::FullscreenWidth = width;
        Window::FullscreenHeight = height;
        return;
    }
    Window::Width = width;
    Window::Height = height;
}
