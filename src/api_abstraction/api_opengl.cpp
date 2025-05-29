#include "api_opengl.hpp"
#include "app/render_manager.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

bool API_GL::Init()
{
    if(is_initialized)
        return true;

    glfwInit();

    if(!CreateMainWindow())
        return false;

    is_initialized = true;
    return true;
}

void API_GL::Shutdown()
{
    glfwTerminate();
}

bool API_GL::CreateMainWindow()
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

    return true;
}