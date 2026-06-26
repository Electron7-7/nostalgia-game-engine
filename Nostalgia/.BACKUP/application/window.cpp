#include "window.hpp"
#include "backends/glfw/glfw_window.hpp"

Unique<IWindow> IWindow::CreateNewWindow(Farg<WindowProperties> inProperties)
{
    // Currently, GLFW is the primary windowing solution and it'll probably stay that way
    // for a while.
    return MakeUnique<WindowGLFW>(inProperties);
}
