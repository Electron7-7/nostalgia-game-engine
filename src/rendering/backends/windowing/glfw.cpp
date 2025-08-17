#include "glfw.hpp"
#include "printing.hpp"
#include "managers/input_manager.hpp"
#include "DearImGui/imgui_impl_glfw.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#define ASSERT_KEY(glfw_key_id) if(!key_ids.contains(glfw_key_id)) return;
#define CONVERT_KEY(glfw_key_id) key_ids.at(glfw_key_id) // FIXME: give this a better name

//--------------------
// PROTOTYPE FUNCTIONS
//--------------------
bool GLFW_Backend::prototype_SetFullscreen(bool is_fullscreen_enabled)
{
    assert(is_initialized);

    if(glfwGetWindowMonitor(glfw_MainWindow))
    bool is_fullscreened = glfwGetWindowMonitor(glfw_MainWindow);
    if(is_fullscreened == Window::Fullscreen) { return; }

    if(is_fullscreened)
    {
        glfwSetWindowMonitor(glfw_MainWindow, nullptr, Window::XPosition, Window::YPosition, Window::Width, Window::Height, GLFW_DONT_CARE);
        Window::Fullscreen = false;
    }

    else
    {
        glfwSetWindowMonitor(glfw_MainWindow, glfw_LastFullscreenedMonitor, 0, 0, Window::FullscreenWidth, Window::FullscreenHeight, GLFW_DONT_CARE);
        Window::Fullscreen = true;
    }

    return main_WindowIsFullscreen;
}
//------------------------
// END PROTOTYPE FUNCTIONS
//------------------------

bool GLFW_Backend::Init()
{
    if(is_initialized)
        return true;

    PRINTDEBUG("GLFW_Backend::Init")

    glfwInit();
    if(!CreateMainWindow())
        return false;

    compatible_graphics_ids = { BackendIDs::OpenGL };
    is_initialized = true;
    return true;
}

bool GLFW_Backend::InitImGui()
{
    if(is_imgui_initialized)
        return true;

    if(!ImGui_ImplGlfw_InitForOpenGL(glfw_MainWindow, true))
    {
        PRINTERROR("GLFW_Backend::InitImGui - ImGui_ImplGlfw_InitForOpenGL returned false!")
        return false;
    }

    is_imgui_initialized = true;
    return true;
}

void GLFW_Backend::Shutdown()
{
    assert(is_initialized);

    ImGui_ImplGlfw_Shutdown();
    glfwTerminate();

    is_initialized = false;
}

void GLFW_Backend::ImGuiNewFrame()
{ ImGui_ImplGlfw_NewFrame(); }

bool GLFW_Backend::CreateMainWindow()
{
    SafeReturn<size_t> new_window = CreateWindow(Window::Name);

    if(new_window.Status() == Status::WindowingBackendWINDOW_CREATION_FAILED)
    {
        PRINT_ERROR("{}", new_window.Status().Printout())
        glfwTerminate();
        return false;
    }

    glfw_MainWindow = glfw_Windows.at(new_window.Data());

    glfwMakeContextCurrent(glfw_MainWindow);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        PRINTERROR("GLFW_Backend::CreateMainWindow - Failed to initialize GLAD!")
        return false;
    }

    if(!Window::Fullscreen)
    {
        int monitor_PositionX = 0;
        int monitor_PositionY = 0;
        glfwGetMonitorPos(glfwGetPrimaryMonitor(), &monitor_PositionX, &monitor_PositionY);

        int window_position_x = (((glfwGetVideoMode(glfwGetPrimaryMonitor())->width - Window::Width) / 2) + monitor_PositionX);
        int window_position_y = (((glfwGetVideoMode(glfwGetPrimaryMonitor())->height - Window::Height) / 2) + monitor_PositionY);
        glfwSetWindowPos(glfw_MainWindow, window_position_x, window_position_y);
    }

    glfw_LastFullscreenedMonitor = glfwGetPrimaryMonitor();

    glfwSetKeyCallback(glfw_MainWindow, GLFW_Backend::glfw_KeyCallbackFunction);
    glfwSetCharCallback(glfw_MainWindow, GLFW_Backend::glfw_CharacterCallbackFunction);
    glfwSetCursorPosCallback(glfw_MainWindow, GLFW_Backend::glfw_CursorPosCallbackFunction);

    return true;
}

SafeReturn<size_t> GLFW_Backend::CreateWindow(const char* window_name)
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

    GLFWwindow* glfw_NewWindow = glfwCreateWindow(window_width, window_height, Window::Name, is_fullscreen, nullptr);

    if(glfw_NewWindow == nullptr)
    {
        PRINTERROR("Failed to create GLFW window!")
        return SafeReturn(glfw_Windows.size() - 1, Status::WindowingBackendWINDOW_CREATION_FAILED);
    }

    glfw_Windows.insert(glfw_Windows.end(), glfw_NewWindow);
    return glfw_Windows.size() - 1;
}

void GLFW_Backend::SwapBuffers()
{ glfwSwapBuffers(glfw_MainWindow); }

void GLFW_Backend::PollEvents()
{ glfwPollEvents(); }

// PRIVATE FUNCTIONS
void GLFW_Backend::glfw_CharacterCallbackFunction(GLFWwindow* window, unsigned int codepoint)
{
    global_InputManager->prototype_CustomCharacterCallback(codepoint);
}

void GLFW_Backend::glfw_KeyCallbackFunction(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ASSERT_KEY(key);

    switch(action)
    {
    case GLFW_RELEASE:
        global_InputManager->Release(CONVERT_KEY(key));
        break;
    case GLFW_PRESS:
        global_InputManager->Press(CONVERT_KEY(key));
        break;
    case GLFW_REPEAT:
        global_InputManager->Repeat(CONVERT_KEY(key));
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
