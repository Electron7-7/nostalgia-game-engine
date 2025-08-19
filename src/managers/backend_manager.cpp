#include "backend_manager.hpp"
#include "DearImGui/imgui.h"
#include "rendering/backends/backend.hpp"
#include "rendering/backends/graphics/opengl.hpp"
#include "rendering/backends/windowing/glfw.hpp"
#include "printing.hpp"

// BackendManager Singleton
BackendManager singleton_BackendManager;
BackendManager* global_BackendManager = &singleton_BackendManager;

// Backend Singletons FIXME: Don't define these here...
OpenGL_Backend singleton_OpenGL_Backend;
GLFW_Backend singleton_GLFW_Backend;

std::map<BackendID, GraphicsBackend*> BackendManager::map_GraphicsBackends = {};
std::map<BackendID, WindowingBackend*> BackendManager::map_WindowingBackends = {};
bool BackendManager::is_initialized = false;
bool BackendManager::is_backend_initialized = false;
bool BackendManager::is_imgui_initialized = false;

BackendID BackendManager::m_DefaultGraphicsID  = BackendIDs::gOpenGL;
BackendID BackendManager::m_DefaultWindowingID = BackendIDs::wGLFW;
BackendID BackendManager::m_CurrentGraphicsID  = m_DefaultGraphicsID;
BackendID BackendManager::m_CurrentWindowingID = m_DefaultWindowingID;

bool BackendManager::Init()
{
    if(is_initialized)
    { return true; }

    // Set up graphics backends map
    map_GraphicsBackends[BackendIDs::gOpenGL] = &singleton_OpenGL_Backend;

    // Set up windowing backends map
    map_WindowingBackends[BackendIDs::wGLFW] = &singleton_GLFW_Backend;

    if(!InitBackend())
    { return false; }

    is_initialized = true;
    return true;
}

void BackendManager::Shutdown()
{
    if(!is_initialized)
    { return; }

    if(is_imgui_initialized)
    { ImGui::DestroyContext(); }

    if(is_backend_initialized)
    {
        GetWindowingBackend()->Shutdown();
        GetGraphicsBackend()->Shutdown();
    }

    is_backend_initialized = false;
    is_imgui_initialized = false;
    is_initialized = false;
}

GraphicsBackend* BackendManager::GetGraphicsBackend()
{
    if(!map_GraphicsBackends.contains(m_CurrentGraphicsID))
    {
        PRINTWARNING("BackendManager::Graphics - current graphics backend ID is invalid; setting it to the default graphics backend ID")
        m_CurrentGraphicsID = m_DefaultGraphicsID;
    }

    return map_GraphicsBackends.at(m_CurrentGraphicsID);
}

WindowingBackend* BackendManager::GetWindowingBackend()
{
    if(!map_WindowingBackends.contains(m_CurrentWindowingID))
    {
        PRINTWARNING("BackendManager::Windowing - current windowing backend ID is invalid; setting it to the default windowing backend ID")
        m_CurrentWindowingID = m_DefaultWindowingID;
    }

    return map_WindowingBackends.at(m_CurrentWindowingID);
}

// TODO: These two functions are more efficient than calling 'Get____Backend()->GetID()', but could it lead to issues?
BackendID BackendManager::GetGraphicsID()
{ return m_CurrentGraphicsID; }

BackendID BackendManager::GetWindowingID()
{ return m_CurrentWindowingID; }

bool BackendManager::InitImGui()
{
    if(is_imgui_initialized)
    { return true; }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    if(!GetWindowingBackend()->InitImGui() || !GetGraphicsBackend()->InitImGui())
    {
        ImGui::DestroyContext();
        return false;
    }

    is_imgui_initialized = true;
    return true;
}

void BackendManager::ImGuiNewFrame()
{
    if(!is_imgui_initialized)
    { return; }

    GetGraphicsBackend()->ImGuiNewFrame();
    GetWindowingBackend()->ImGuiNewFrame();

    ImGui::NewFrame();
}

void BackendManager::ImGuiRender()
{
    if(!is_imgui_initialized)
    { return; }

    ImGui::Render();
    GetGraphicsBackend()->ImGuiRender();
}

bool BackendManager::InitBackend()
{
    if(is_backend_initialized)
    { return true; }

    if(!GetWindowingBackend()->Init() || !GetGraphicsBackend()->Init())
    { return false; }

    if(!GetWindowingBackend()->CompatibleWith(m_CurrentGraphicsID))
    {
        // TODO: Figure out what I wanna do if this happens (probably just load the default backends and try again)
        PRINTERROR("BackendManager::InitBackend - current graphics backend & windowing backend are not compatible with eachother! (currently, the only existing backends are OpenGL and GLFW, so you should NOT see this message)")
        return false;
    }

    is_backend_initialized = true;
    return true;
}

void BackendManager::UpdateWindowState()
{ GetWindowingBackend()->UpdateState(); }
