#ifndef GLFW_BACKEND_H
#define GLFW_BACKEND_H

#include "windowing.hpp"
#include "common/ids.hpp"
#include "frozen/map.h"

struct GLFWwindow;  // Forward Declaration
struct GLFWmonitor; // Forward Declaration

class GLFW_Backend : public WindowingBackend
{
public:
    bool Init();
    void Shutdown();
    BackendID GetID() final
    { return gBackendIDs::wGLFW; }

    bool InitImGui();
    void ImGuiNewFrame();

    SafeStatus CreateMainWindow();

    void ResizeWindow(int Width, int Height);
    void MoveWindow(int XPosition, int YPosition);
    void ToggleFullscreen();
    void ToggleRawMouseMotion();
    MouseMode ToggleMouseMode(MouseMode, MouseMode);
    void SetFullscreen(bool FullscreenOn);
    void SetRawMouseMotion(bool RawMotionOn);
    bool SetMouseMode(MouseMode);
    MouseMode GetMouseMode();

    void GetMousePosition(glm::vec2&);
    bool UpdateBinding(InputBinding&);
    void SwapBuffers();
    void PollEvents();
    void UpdateState();

private:
    GLFWwindow* m_MainWindow = nullptr;
    GLFWmonitor* m_LastFullscreenedMonitor = nullptr;

    // GLFW callback functions
    static void glfw_WindowPositionCallbackFunction(GLFWwindow*, int, int);
    static void glfw_WindowResizeCallbackFunction(GLFWwindow*, int, int);

    // -1 because GLFW doesn't have an ID for the `fn` key
    static constinit const frozen::map<ID, id_t, BindingIDs::KeyIDsCount + BindingIDs::MouseButtonIDsCount - 1>
        s_cInputIdToGlfw;
};

#endif // GLFW_BACKEND_H
