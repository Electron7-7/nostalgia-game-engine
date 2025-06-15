#include "backend_manager.hpp"
#include "DearImGui/imgui.h"
#include "backends/backend.hpp"
#include "backends/graphics/opengl.hpp"
#include "backends/windowing/glfw.hpp"
#include "common/debugging.hpp"

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
GraphicsBackendID BackendManager::current_GraphicsBackend = BackendIDs::default_Graphics;
WindowingBackendID BackendManager::current_WindowingBackend = BackendIDs::default_Windowing;


//--------------------
// PROTOTYPE FUNCTIONS
//--------------------



//------------------------
// END PROTOTYPE FUNCTIONS
//------------------------

bool BackendManager::Init()
{
    if(is_initialized)
        return true;

    // Set up graphics backends map
    map_GraphicsBackends[BackendIDs::OpenGL] = &singleton_OpenGL_Backend;

    // Set up windowing backends map
    map_WindowingBackends[BackendIDs::GLFW] = &singleton_GLFW_Backend;

    is_initialized = true;

    if(!InitBackend())
        return false;

    return true;
}

GraphicsBackend* BackendManager::GetGraphicsBackend()
{
    if(!map_GraphicsBackends.contains(current_GraphicsBackend))
    {
        PRINTWARN("BackendManager::Graphics - current graphics backend ID is invalid; setting it to the default graphics backend ID");
        current_GraphicsBackend = BackendIDs::default_Graphics;
    }

    return map_GraphicsBackends.at(current_GraphicsBackend);
}

WindowingBackend* BackendManager::GetWindowingBackend()
{
    if(!map_WindowingBackends.contains(current_WindowingBackend))
    {
        PRINTWARN("BackendManager::Windowing - current windowing backend ID is invalid; setting it to the default windowing backend ID");
        current_WindowingBackend = BackendIDs::default_Windowing;
    }

    return map_WindowingBackends.at(current_WindowingBackend);
}

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

    if(!GetWindowingBackend()->CompatibleWith(current_GraphicsBackend))
    {
        // TODO: Figure out what I wanna do if this happens (probably just load the default backends and try again)
        PRINTERR("BackendManager::InitBackend - current graphics backend & windowing backend are not compatible with eachother! (currently, the only existing backends are OpenGL and GLFW, so you should NOT see this message)");
        return false;
    }

    is_backend_initialized = true;
    return true;
}

// FIXME: I'm like 99% sure that if you were to actually try and do this, you'd crash or some other nasty outcome, but there's only one backend rn so...
/*int BackendManager::RequestBackendChange(int backend_id)
{
    if(!map_GraphicsBackends.contains(backend_id) || !map_WindowingBackends.contains(backend_id))
    {
        PRINTWARN("BackendManager::RequestBackendChange - supplied backend ID invalid; returning previous backend ID");
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
            PRINTERR("BackendManager::RequestBackendChange - default backend(s) failed to initialize!");
            return -1; // FIXME: Probably not a good return value
        }

        PRINTWARN("BackendManager::RequestBackendChange - requested backend failed to initialize! Changing to default backend");
        RequestBackendChange(BackendIDs::Default);
    }

    return current_backend;
}*/
