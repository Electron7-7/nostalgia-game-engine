#include "glfw.hpp"
#include "common/debugging.hpp"
#include "managers/input_manager.hpp"
#include "common/opengl_includes.hpp"

#define ASSERT_KEY(glfw_key_id) if(!key_id_map.contains(glfw_key_id)) return;
#define CONVERT_KEY(glfw_key_id) key_id_map.at(glfw_key_id) // FIXME: give this a better name

//--------------------
// PROTOTYPE FUNCTIONS
//--------------------
bool GLFW_Backend::prototype_SetFullscreen(bool is_fullscreen_enabled)
{
    assert(is_initialized);

    if(glfwGetWindowMonitor(glfw_MainWindow))
    {
        glfwSetWindowMonitor(glfw_MainWindow, nullptr, main_WindowPositionX, main_WindowPositionY, main_WindowWidth, main_WindowHeight, GLFW_DONT_CARE);
        main_WindowIsFullscreen = false;
    }

    else
    {
        glfwSetWindowMonitor(glfw_MainWindow, glfw_LastFullscreenedMonitor, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE);
        main_WindowIsFullscreen = true;
    }

    return main_WindowIsFullscreen;

}

void GLFW_Backend::prototype_SwapBuffers()
{
    glfwSwapBuffers(glfw_MainWindow);
}

void GLFW_Backend::prototype_PollEvents()
{
    glfwPollEvents();
}
//------------------------
// END PROTOTYPE FUNCTIONS
//------------------------


bool GLFW_Backend::Init()
{
    if(is_initialized)
        return true;

    PRINTDEBUG("GLFW_Backend::Init");

    glfwInit();
    if(!CreateMainWindow())
        return false;

    compatible_graphics_ids = { BackendIDs::OpenGL };
    is_initialized = true;
    return true;
}

void GLFW_Backend::Shutdown()
{
    assert(is_initialized);

    glfwTerminate();

    is_initialized = false;
}

bool GLFW_Backend::CreateMainWindow()
{
    unsigned int new_window = CreateWindow(main_WindowName);
    if(new_window == WindowingBackend::WINDOW_CREATION_FAILED)
    {
        glfwTerminate();
        return false;
    }

    glfw_MainWindow = glfw_Windows.at(new_window);

    glfwMakeContextCurrent(glfw_MainWindow);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        PRINTERR("Failed to initialize GLAD!");
        return false;
    }

    if(!main_WindowIsFullscreen && main_WindowIsCentered)
    {
        int monitor_PositionX = 0;
        int monitor_PositionY = 0;
        glfwGetMonitorPos(glfwGetPrimaryMonitor(), &monitor_PositionX, &monitor_PositionY);

        int window_position_x = (((glfwGetVideoMode(glfwGetPrimaryMonitor())->width - main_WindowWidth) / 2) + monitor_PositionX);
        int window_position_y = (((glfwGetVideoMode(glfwGetPrimaryMonitor())->height - main_WindowHeight) / 2) + monitor_PositionY);
        glfwSetWindowPos(glfw_MainWindow, window_position_x, window_position_y);
    }

    glfw_LastFullscreenedMonitor = glfwGetPrimaryMonitor();

    glfwSetKeyCallback(glfw_MainWindow, GLFW_Backend::glfw_KeyCallbackFunction);
    glfwSetCharCallback(glfw_MainWindow, GLFW_Backend::glfw_CharacterCallbackFunction);
    glfwSetCursorPosCallback(glfw_MainWindow, GLFW_Backend::glfw_CursorPosCallbackFunction);

    return true;
}

int GLFW_Backend::CreateWindow(const char* window_name)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* is_fullscreen = (main_WindowIsFullscreen) ? glfwGetPrimaryMonitor() : nullptr;
    GLFWwindow* glfw_NewWindow = glfwCreateWindow(main_WindowWidth, main_WindowHeight, window_name, is_fullscreen, nullptr);

    if(glfw_NewWindow == nullptr)
    {
        PRINTERR("Failed to create GLFW window!");
        return WindowingBackend::WINDOW_CREATION_FAILED;
    }

    glfw_Windows.insert(glfw_Windows.end(), glfw_NewWindow);
    return glfw_Windows.size() - 1;
}

void GLFW_Backend::glfw_CharacterCallbackFunction(GLFWwindow* window, unsigned int codepoint)
{
    global_InputManager->prototype_CustomCharacterCallback(codepoint);
}

void GLFW_Backend::glfw_KeyCallbackFunction(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ASSERT_KEY(key);

    if(action == 1)
        global_KeyHandler->Press(CONVERT_KEY(key));

    else if(action == 0)
        global_KeyHandler->Release(CONVERT_KEY(key));
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
