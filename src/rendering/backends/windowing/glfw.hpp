#ifndef GLFW_BACKEND_H
#define GLFW_BACKEND_H

#include "input/key.hpp"
#include "rendering/backends/backend.hpp"

#include <map>

struct GLFWwindow; // Forward Declaration
struct GLFWmonitor; // Forward Declaration

class GLFW_Backend : public WindowingBackend
{
public:
    bool Init();
    void Shutdown();
    BackendID GetID() { return BackendIDs::wGLFW; }

    bool InitImGui();
    void ImGuiNewFrame();

    SafeStatus CreateMainWindow();

    void ResizeWindow(int Width, int Height);
    void MoveWindow(int XPosition, int YPosition);
    void SetFullscreen(bool FullscreenOn);

    void SwapBuffers();
    void PollEvents();
    void UpdateState();

private:
    GLFWwindow* m_MainWindow = nullptr;
    GLFWmonitor* m_LastFullscreenedMonitor = nullptr;

    // GLFW callback functions
    static void glfw_KeyCallbackFunction(GLFWwindow*, int, int, int, int);
    static void glfw_CharacterCallbackFunction(GLFWwindow*, unsigned int);
    static void glfw_CursorPosCallbackFunction(GLFWwindow*, double, double);
    static void glfw_WindowPositionCallbackFunction(GLFWwindow*, int, int);
    static void glfw_WindowResizeCallbackFunction(GLFWwindow*, int, int);

    typedef unsigned int GLFW_KeyID;
    static const std::map<GLFW_KeyID, KeyID> key_ids;
};

#endif // GLFW_BACKEND_H
