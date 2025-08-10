#ifndef GLFW_BACKEND_H
#define GLFW_BACKEND_H

#include "input/key.hpp"
#include "rendering/backends/backend.hpp"
#include "common/macros.hpp"

#include <vector>
#include <map>

FORWARD_DECLARE(struct GLFWwindow;)
FORWARD_DECLARE(struct GLFWmonitor;)

class GLFW_Backend : public WindowingBackend
{
public:
    virtual bool Init();
    virtual bool InitImGui();
    virtual void Shutdown();
    virtual void ImGuiNewFrame();
    virtual bool CreateMainWindow();
    virtual SafeReturn<size_t> CreateWindow(const char* WindowName);

    virtual void SwapBuffers();
    virtual void PollEvents();

    // PROTOTYPE FUNCTIONS (inherited from Backend)
    virtual bool prototype_SetFullscreen(bool FullscreenOn);

private:
    GLFWwindow* glfw_MainWindow = nullptr;
    GLFWmonitor* glfw_LastFullscreenedMonitor = nullptr;
    std::vector<GLFWwindow*> glfw_Windows = {};

    // GLFW callback functions
    static void glfw_KeyCallbackFunction(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods);
    static void glfw_CharacterCallbackFunction(GLFWwindow* Window, unsigned int CodePoint);
    static void glfw_CursorPosCallbackFunction(GLFWwindow* Window, double XPosition, double YPosition);

    typedef unsigned int GLFW_KeyID;
    static const std::map<GLFW_KeyID, KeyID> key_ids;
};

extern GLFW_Backend singleton_GLFW_Backend;

#endif // GLFW_BACKEND_H
