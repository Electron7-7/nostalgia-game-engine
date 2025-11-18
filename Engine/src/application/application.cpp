#include "application.hpp"
#include "managers/manager.hpp"
#include "managers/physics_manager.hpp"
#include "managers/theatre_manager.hpp"
#include "managers/render_manager.hpp"
#include "managers/input_manager.hpp"
#include "managers/ui_manager.hpp"
#include "window.hpp"
#include "backends/glfw/glfw_window.hpp" // GLFW is the main windowing solution for now (possibly permanent)

#include <cassert>

IApplication* g_pApplication{nullptr};

IApplication::IApplication()
{
    assert(!g_pApplication && "Only one IApplication instance allowed!");
    g_pApplication = this;
    IManager::Add(g_pPhysicsManager);
    IManager::Add(g_pTheatreManager);
    IManager::Add(g_pRenderManager);
    IManager::Add(g_pInputManager);
    IManager::Add(g_pUIManager);
    mMainWindow = IWindow::CreateWindow<WindowGLFW>(); // WindowProperties default constructor
}
