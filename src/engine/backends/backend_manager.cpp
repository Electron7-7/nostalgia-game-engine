#include "backends.hpp"
#include "backend_manager.hpp"
#include "graphics/opengl.hpp"
#include "windowing/glfw.hpp"
#include "debugging.hpp"

// BackendManager Singleton
BackendManager singleton_BackendManager;
BackendManager* global_BackendManager = &singleton_BackendManager;

// Backend Singletons
OpenGL_Backend singleton_OpenGL_Backend;
GLFW_Backend singleton_GLFW_Backend;

std::map<int, GraphicsBackend*> BackendManager::map_GraphicsBackends = {};
std::map<int, WindowingBackend*> BackendManager::map_WindowingBackends = {};
bool BackendManager::is_initialized = false;
bool BackendManager::is_backend_initialized = false;
int BackendManager::current_backend = BackendIDs::Default;


//--------------------
// PROTOTYPE FUNCTIONS
//--------------------

void BackendManager::prototype_RenderFrame()
{
    // These render steps can (and should) be changed in the future

    // Step 1: Swap & clear the buffer and poll input events
    GetWindowingBackend()->prototype_SwapBuffers();
    GetGraphicsBackend()->prototype_ClearBuffer(glm::vec4(0.9f, 0.1f, 1.0f, 1.0f)); // TODO: replace hardcoded clear color with variable(s)
    GetWindowingBackend()->prototype_PollEvents();

    // Step 2: Render frame
    GetGraphicsBackend()->prototype_RenderFrame();
}

//------------------------
// END PROTOTYPE FUNCTIONS
//------------------------

bool BackendManager::Init()
{
    if(is_initialized)
        return true;

    // Set up graphics backends map
    map_GraphicsBackends[BackendIDs::OpenGL_GLFW] = &singleton_OpenGL_Backend;

    // Set up windowing backends map
    map_WindowingBackends[BackendIDs::OpenGL_GLFW] = &singleton_GLFW_Backend;

    is_initialized = true;

    if(!InitBackend())
        return false;

    return true;
}

int BackendManager::GetBackendID()
{ return current_backend; }

GraphicsBackend* BackendManager::GetGraphicsBackend()
{
    if(!map_GraphicsBackends.contains(current_backend))
    {
        PRINTWARN("BackendManager::Graphics - current backend ID is invalid; returning default backend ID");
        current_backend = BackendIDs::Default;
    }

    return map_GraphicsBackends.at(current_backend);
}

WindowingBackend* BackendManager::GetWindowingBackend()
{
    if(!map_WindowingBackends.contains(current_backend))
    {
        PRINTWARN("BackendManager::Windowing - current backend ID is invalid; returning default backend ID");
        current_backend = BackendIDs::Default;
    }

    return map_WindowingBackends.at(current_backend);
}


int BackendManager::RequestBackendChange(int backend_id)
{
    // FIXME: I'm like 99% sure that if you were to actually try and do this, you'd crash or some other nasty outcome, but there's only one backend rn so...

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
}

bool BackendManager::InitBackend()
{
    if(is_backend_initialized)
        return true;

    if(!GetWindowingBackend()->Init() || !GetGraphicsBackend()->Init())
        return false;

    GetWindowingBackend()->CreateMainWindow();

    is_backend_initialized = true;
    return true;
}