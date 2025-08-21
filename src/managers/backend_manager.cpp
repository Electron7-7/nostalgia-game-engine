#include "backend_manager.hpp"
#include "DearImGui/imgui.h"
#include "rendering/backends/backend.hpp"
#include "rendering/backends/graphics/opengl.hpp"
#include "rendering/backends/windowing/glfw.hpp"
#include "printing.hpp"

#include <map>

static std::map<BackendID, GraphicsBackend*>  map_GraphicsBackends  = {};
static std::map<BackendID, WindowingBackend*> map_WindowingBackends = {};

static BackendID m_DefaultGraphicsID  = BackendIDs::gOpenGL;
static BackendID m_DefaultWindowingID = BackendIDs::wGLFW;
static BackendID m_CurrentGraphicsID  = m_DefaultGraphicsID;
static BackendID m_CurrentWindowingID = m_DefaultWindowingID;

static BackendManager s_BackendManager;
BackendManager* g_pBackendManager = &s_BackendManager;

// FIXME: Don't define these here...
static OpenGL_Backend s_OpenGL_Backend;
static GLFW_Backend s_GLFW_Backend;

bool BackendManager::Init()
{
    // Set up graphics backends map
    map_GraphicsBackends[BackendIDs::gOpenGL] = &s_OpenGL_Backend;

    // Set up windowing backends map
    map_WindowingBackends[BackendIDs::wGLFW] = &s_GLFW_Backend;

    return(InitBackend());
}

void BackendManager::Shutdown()
{
    GetWindowingBackend()->Shutdown();
    GetGraphicsBackend()->Shutdown();

    if(m_IsImguiInitialized)
    { ImGui::DestroyContext(); }

    m_IsImguiInitialized = false;
}

GraphicsBackend* BackendManager::GetGraphicsBackend()
{
    if(!map_GraphicsBackends.contains(m_CurrentGraphicsID))
    {
        PRINT_WARNING("BackendManager::Graphics - current graphics backend ID is invalid; setting it to the default graphics backend ID")
        m_CurrentGraphicsID = m_DefaultGraphicsID;
    }

    return map_GraphicsBackends.at(m_CurrentGraphicsID);
}

WindowingBackend* BackendManager::GetWindowingBackend()
{
    if(!map_WindowingBackends.contains(m_CurrentWindowingID))
    {
        PRINT_WARNING("BackendManager::Windowing - current windowing backend ID is invalid; setting it to the default windowing backend ID")
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
    if(m_IsImguiInitialized)
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

    m_IsImguiInitialized = true;
    return true;
}

void BackendManager::ImGuiNewFrame()
{
    if(!m_IsImguiInitialized)
    { return; }

    GetGraphicsBackend()->ImGuiNewFrame();
    GetWindowingBackend()->ImGuiNewFrame();

    ImGui::NewFrame();
}

void BackendManager::ImGuiRender()
{
    if(!m_IsImguiInitialized)
    { return; }

    ImGui::Render();
    GetGraphicsBackend()->ImGuiRender();
}

bool BackendManager::InitBackend()
{
    if(!GetWindowingBackend()->Init() || !GetGraphicsBackend()->Init())
    { return false; }

    if(!GetWindowingBackend()->CompatibleWith(m_CurrentGraphicsID))
    {
        // TODO: Figure out what I wanna do if this happens (probably just load the default backends and try again)
        PRINT_ERROR("BackendManager::InitBackend - current graphics backend & windowing backend are not compatible with eachother! (currently, the only existing backends are OpenGL and GLFW, so you should NOT see this message)")
        return false;
    }

    return true;
}

void BackendManager::UpdateWindowState()
{ GetWindowingBackend()->UpdateState(); }
