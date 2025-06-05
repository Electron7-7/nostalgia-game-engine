#include "glfw.hpp"
#include "debugging.hpp"
#include "opengl_includes.hpp"
#include "engine/managers/render_manager.hpp"
#include "engine/managers/input_manager.hpp"

#define ASSERT_KEY(glfw_key_id) if(!key_id_map.contains(glfw_key_id)) return;
#define CONVERT_KEY(glfw_key_id) key_id_map.at(glfw_key_id) // FIXME: give this a better name

//--------------------
// PROTOTYPE FUNCTIONS
//--------------------
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

    glfwInit();

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* is_fullscreen = (RenderManager::is_main_window_fullscreen) ? glfwGetPrimaryMonitor() : nullptr;

    glfw_MainWindow = glfwCreateWindow(RenderManager::main_WindowWidth, RenderManager::main_WindowHeight, RenderManager::main_WindowName, is_fullscreen, nullptr);

    if(glfw_MainWindow == NULL)
    {
        // PRINTERR("Failed to create GLFW window!")
        glfwTerminate();
    }

    glfwMakeContextCurrent(glfw_MainWindow);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        // PRINTERR("Failed to initialize GLAD!")
        return false;
    }

    if(!RenderManager::is_main_window_fullscreen && RenderManager::center_main_window_if_windowed)
    {
        int monitor_PositionX = 0;
        int monitor_PositionY = 0;
        glfwGetMonitorPos(glfwGetPrimaryMonitor(), &monitor_PositionX, &monitor_PositionY);

        int window_position_x = (((glfwGetVideoMode(glfwGetPrimaryMonitor())->width - RenderManager::main_WindowWidth) / 2) + monitor_PositionX);
        int window_position_y = (((glfwGetVideoMode(glfwGetPrimaryMonitor())->height - RenderManager::main_WindowHeight) / 2) + monitor_PositionY);
        glfwSetWindowPos(glfw_MainWindow, window_position_x, window_position_y);
    }

    glfwSetKeyCallback(glfw_MainWindow, GLFW_Backend::glfw_KeyCallbackFunction);
    glfwSetCharCallback(glfw_MainWindow, GLFW_Backend::glfw_CharacterCallbackFunction);
    glfwSetCursorPosCallback(glfw_MainWindow, GLFW_Backend::glfw_CursorPosCallbackFunction);

    return true;
}

void GLFW_Backend::glfw_CharacterCallbackFunction(GLFWwindow* window, unsigned int codepoint)
{
    PRINTDEBUG(std::string("Key pressed: ").append(1, static_cast<char>(codepoint)));
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