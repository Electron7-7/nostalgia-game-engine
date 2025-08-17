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
    virtual bool Init();
    virtual bool InitImGui();
    virtual void Shutdown();
    virtual void ImGuiNewFrame();
    virtual bool CreateMainWindow();
    virtual SafeReturn<size_t> CreateWindow(const char* WindowName);

    virtual void ResizeWindow(int Width, int Height);
    virtual void MoveWindow(int XPosition, int YPosition);

    virtual void SwapBuffers();
    virtual void PollEvents();
    virtual void UpdateState();

    // PROTOTYPE FUNCTIONS (inherited from Backend)
    virtual void prototype_SetFullscreen(bool FullscreenOn);

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
