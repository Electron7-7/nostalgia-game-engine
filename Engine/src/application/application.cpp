#include "application.hpp"
#include "managers/manager.hpp"
#include "managers/physics_manager.hpp"
#include "managers/theatre_manager.hpp"
#include "managers/render_manager.hpp"
#include "managers/input_manager.hpp"
#include "managers/event_manager.hpp"
#include "managers/ui_manager.hpp"
#include "window.hpp"
#include "backends/glfw/glfw_window.hpp" // GLFW is the main windowing solution for now (possibly permanent)

#include <cassert>

inline static IApplication* s_pInstance{nullptr};

IApplication* Application()
{ return s_pInstance; }

IWindow& MainWindow()
{ return Application()->GetWindow(); }

IApplication::IApplication()
{
    assert(!s_pInstance && "Only one Application instance allowed!");
    s_pInstance = this;
    IManager::Add(g_pPhysicsManager);
    IManager::Add(g_pTheatreManager);
    IManager::Add(g_pRenderManager);
    IManager::Add(g_pInputManager);
    IManager::Add(g_pEventManager);
    IManager::Add(g_pUIManager);
    mMainWindow = IWindow::CreateWindow<WindowGLFW>(); // Using the default `WindowProperties` constructor
}
