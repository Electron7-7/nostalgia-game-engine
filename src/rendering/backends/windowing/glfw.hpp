#ifndef GLFW_BACKEND_H
#define GLFW_BACKEND_H

#include "input/key.hpp"
#include "rendering/backends/backend.hpp"

#include <vector>
#include <map>

struct GLFWwindow; // Forward Declaration
struct GLFWmonitor; // Forward Declaration

class GLFW_Backend : public WindowingBackend
{
public:
    virtual bool CreateMainWindow();
    virtual SafeReturn<size_t> CreateWindow(const char* WindowName);
    bool Init();
    void Shutdown();

    bool InitImGui();
    void ImGuiNewFrame();


    void ResizeWindow(int Width, int Height);
    void MoveWindow(int XPosition, int YPosition);
    void SetFullscreen(bool FullscreenOn);

    void SwapBuffers();
    void PollEvents();
    void UpdateState();

private:
    GLFWwindow* glfw_MainWindow = nullptr;
    GLFWmonitor* glfw_LastFullscreenedMonitor = nullptr;
    std::vector<GLFWwindow*> glfw_Windows = {};

    // GLFW callback functions
    static void glfw_KeyCallbackFunction(GLFWwindow*, int, int, int, int);
    static void glfw_CharacterCallbackFunction(GLFWwindow*, unsigned int);
    static void glfw_CursorPosCallbackFunction(GLFWwindow*, double, double);
    static void glfw_WindowPositionCallbackFunction(GLFWwindow*, int, int);
    static void glfw_WindowResizeCallbackFunction(GLFWwindow*, int, int);

    typedef unsigned int GLFW_KeyID;
    static const std::map<GLFW_KeyID, KeyID> key_ids;
};

extern GLFW_Backend singleton_GLFW_Backend;

#endif // GLFW_BACKEND_H
