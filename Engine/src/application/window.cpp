#include "window.hpp"
#include "managers/event_manager"

void IWindow::SetInputEventCallback(InputEventCallbackFunction inCallback)
{ g_pEventManager->SetInputEventCallback(inCallback); }
