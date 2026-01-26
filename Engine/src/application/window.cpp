#include "window.hpp"

Size2D IWindow::GetScale() const
{ return Size2D{mData.width, mData.height}; }

Position2D IWindow::GetPosition() const
{ return Position2D{mData.x_pos, mData.y_pos}; }
