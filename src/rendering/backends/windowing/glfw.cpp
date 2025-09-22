#include "glfw.hpp"
#include "debug.hpp"
#include "managers/backend_manager.hpp"
#include "managers/input_manager.hpp"
#include "settings/settings.hpp"
#include "DearImGui/imgui_impl_glfw.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#define ASSERT_KEY(glfw_key_id) if(!key_ids.contains(glfw_key_id)) return;
#pragma message("(FIXME) give this a better name")
#define CONVERT_KEY(glfw_key_id) key_ids.at(glfw_key_id)

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

    compatible_graphics_backends = { BackendIDs::gOpenGL };
    m_IsInitialized = true;
    return true;
}

bool GLFW_Backend::InitImGui()
{
    if(is_imgui_initialized)
    { return true; }

    switch(g_pBackendManager->GetGraphicsID())
    {
    case BackendIDs::gOpenGL:
        if(!ImGui_ImplGlfw_InitForOpenGL(m_MainWindow, true))
        {
            PRINT_ERROR("GLFW_Backend::InitImGui - ImGui_ImplGlfw_InitForOpenGL returned false!")
            return false;
        }
        break;
    }

    is_imgui_initialized = true;
    return true;
}

void GLFW_Backend::Shutdown()
{
    assert(m_IsInitialized);

    if(is_imgui_initialized)
    { ImGui_ImplGlfw_Shutdown(); }

    glfwTerminate();

    m_IsInitialized = false;
    is_imgui_initialized = false;

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
    glfwSetKeyCallback(m_MainWindow, GLFW_Backend::glfw_KeyCallbackFunction);
    glfwSetCharCallback(m_MainWindow, GLFW_Backend::glfw_CharacterCallbackFunction);
    glfwSetCursorPosCallback(m_MainWindow, GLFW_Backend::glfw_CursorPosCallbackFunction);

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

void GLFW_Backend::SetFullscreen(bool is_fullscreen_enabled)
{
    assert(m_IsInitialized);

    bool is_fullscreened = glfwGetWindowMonitor(m_MainWindow);
    if(is_fullscreened == Window::Fullscreen)
    { return; }

    if(is_fullscreened)
    {
        glfwSetWindowMonitor(m_MainWindow, nullptr, Window::XPosition, Window::YPosition, Window::Width, Window::Height, GLFW_DONT_CARE);
        Window::Fullscreen = false;
    }

    else
    {
        glfwSetWindowMonitor(m_MainWindow, m_LastFullscreenedMonitor, 0, 0, Window::FullscreenWidth, Window::FullscreenHeight, GLFW_DONT_CARE);
        Window::Fullscreen = true;
    }
}

void GLFW_Backend::SwapBuffers()
{ glfwSwapBuffers(m_MainWindow); }

void GLFW_Backend::PollEvents()
{ glfwPollEvents(); }

void GLFW_Backend::UpdateState()
{
    SetFullscreen(Window::Fullscreen);

    if(Window::Fullscreen)
    {
        ResizeWindow(Window::FullscreenWidth, Window::FullscreenHeight);
        return;
    }

    ResizeWindow(Window::Width, Window::Height);
    MoveWindow(Window::XPosition, Window::YPosition);
}

// PRIVATE FUNCTIONS
void GLFW_Backend::glfw_CharacterCallbackFunction(GLFWwindow* window, unsigned int codepoint)
{ g_pInputManager->prototype_CustomCharacterCallback(codepoint); }

void GLFW_Backend::glfw_KeyCallbackFunction(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ASSERT_KEY(key);

    switch(action)
    {
    case GLFW_RELEASE:
        g_pInputManager->Release(CONVERT_KEY(key));
        break;
    case GLFW_PRESS:
        g_pInputManager->Press(CONVERT_KEY(key));
        break;
    case GLFW_REPEAT:
        g_pInputManager->Repeat(CONVERT_KEY(key));
        break;
    }
}

void GLFW_Backend::glfw_CursorPosCallbackFunction(GLFWwindow* window, double position_x, double position_y)
{
    /*
    For raw mouse motion, use:
    --------------------------
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    */
}

void GLFW_Backend::glfw_WindowPositionCallbackFunction(GLFWwindow* window, int position_x, int position_y)
{
    Window::XPosition = position_x;
    Window::YPosition = position_y;
}

void GLFW_Backend::glfw_WindowResizeCallbackFunction(GLFWwindow* window, int width, int height)
{
    if(Window::Fullscreen)
    {
        Window::FullscreenWidth = width;
        Window::FullscreenHeight = height;
        return;
    }

    Window::Width = width;
    Window::Height = height;
}
