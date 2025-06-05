#ifndef GLFW_BACKEND_H
#define GLFW_BACKEND_H

#include "engine/backends/backend.hpp"
#include "engine/input/key_handler.hpp"
#include "common/macros.hpp"

FORWARD_DECLARE(struct GLFWwindow;)

class GLFW_Backend : public WindowingBackend
{
public:
    virtual bool Init();
    virtual void Shutdown();
    virtual bool CreateMainWindow();

    // PROTOTYPE FUNCTIONS
    virtual void prototype_SwapBuffers();
    virtual void prototype_PollEvents();

private:
    GLFWwindow* glfw_MainWindow = nullptr;

    // GLFW callback functions
    static void glfw_KeyCallbackFunction(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods);
    static void glfw_CharacterCallbackFunction(GLFWwindow* Window, unsigned int CodePoint);
    static void glfw_CursorPosCallbackFunction(GLFWwindow* Window, double XPosition, double YPosition);

    typedef unsigned int GLFW_KeyID;
    static const std::map<GLFW_KeyID, KeyID> key_id_map;
};

extern GLFW_Backend singleton_GLFW_Backend;

#endif // GLFW_BACKEND_H