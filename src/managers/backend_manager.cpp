#include "backend_manager.hpp"
#include "DearImGui/imgui.h"
#include "rendering/backends/backend.hpp"
#include "rendering/backends/graphics/opengl.hpp"
#include "rendering/backends/windowing/glfw.hpp"
#include "printing.hpp"

// BackendManager Singleton
BackendManager singleton_BackendManager;
BackendManager* global_BackendManager = &singleton_BackendManager;

// Backend Singletons
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
        return true;

    // Set up graphics backends map
    map_GraphicsBackends[BackendIDs::gOpenGL] = &singleton_OpenGL_Backend;

    // Set up windowing backends map
    map_WindowingBackends[BackendIDs::wGLFW] = &singleton_GLFW_Backend;

    if(!InitBackend())
        return false;

    is_initialized = true;
    return true;
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
        return true;

    if(!GetWindowingBackend()->InitImGui())
        return false;

    if(!GetGraphicsBackend()->InitImGui())
        return false;

    is_imgui_initialized = true;
    return true;
}

void BackendManager::ImGuiNewFrame()
{
    if(!is_imgui_initialized && !InitImGui())
        return;

    GetGraphicsBackend()->ImGuiNewFrame();
    GetWindowingBackend()->ImGuiNewFrame();
    ImGui::NewFrame();
}

void BackendManager::ImGuiRender()
{
    if(!is_imgui_initialized && !InitImGui())
        return;

    ImGui::Render();
    GetGraphicsBackend()->ImGuiRender();
}


bool BackendManager::InitBackend()
{
    if(is_backend_initialized)
        return true;

    if(!GetWindowingBackend()->Init() || !GetGraphicsBackend()->Init())
        return false;

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

// FIXME: I'm like 99% sure that if you were to actually try and do this, you'd crash or some other nasty outcome, but there's only one backend rn so...
/*int BackendManager::RequestBackendChange(int backend_id)
{
    if(!map_GraphicsBackends.contains(backend_id) || !map_WindowingBackends.contains(backend_id))
    {
        PRINT_WARNING("BackendManager::RequestBackendChange - supplied backend ID invalid; returning previous backend ID")
        return current_backend;
    }

    GetGraphicsBackend()->Shutdown();
    GetWindowingBackend()->Shutdown();

    current_backend = backend_id;
    is_backend_initialized = false;

    if(!InitBackend())
    {
        if(current_backend == BackendIDs::Default)
        {
            PRINT_ERROR("BackendManager::RequestBackendChange - default backend(s) failed to initialize!")
            return -1; // FIXME: Probably not a good return value
        }

        PRINT_WARNING("BackendManager::RequestBackendChange - requested backend failed to initialize! Changing to default backend")
        RequestBackendChange(BackendIDs::Default);
    }

    return current_backend;
}*/
