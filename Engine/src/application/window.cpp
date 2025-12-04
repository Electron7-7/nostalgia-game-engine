#include "window.hpp"
#include "managers/event_manager.hpp"
#include "math/containers.hpp"

Scale2D IWindow::GetScale() const
{ return Scale2D{mData.width, mData.height}; }

Position2D IWindow::GetPosition() const
{ return Position2D{mData.x_pos, mData.y_pos}; }

void IWindow::SetInputEventCallback(InputEventCallback inCallback)
{ g_pEventManager->SetInputEventCallback(inCallback); }
