#include "window.hpp"
#include "backends/glfw/glfw_window.hpp"

Size2D IWindow::GetScale() const
{ return Size2D{mData.width, mData.height}; }

Position2D IWindow::GetPosition() const
{ return Position2D{mData.x_pos, mData.y_pos}; }

Unique<IWindow> IWindow::CreateWindow(Farg<WindowProperties> inProperties)
{
    // Currently, GLFW is the primary windowing solution and it'll probably stay that way
    // for a while.
    return MakeUnique<WindowGLFW>(inProperties);
}
