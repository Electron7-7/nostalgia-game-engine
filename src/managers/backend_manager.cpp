#include "backend_manager.hpp"
#include "settings/settings.hpp"
#include "DearImGui/imgui.h"
#include "printing.hpp"
// Include all backends here
#include "rendering/backends/graphics/opengl.hpp"
#include "rendering/backends/windowing/glfw.hpp"

static BackendManager s_BackendManager;
BackendManager* g_pBackendManager = &s_BackendManager;

static std::shared_ptr<GraphicsBackend>  s_Graphics  = nullptr;
static std::shared_ptr<WindowingBackend> s_Windowing = nullptr;

static void CreateGraphicsBackend()
{
    switch(Settings::Engine::GraphicsBackend)
    {
    case gBackendIDs::gOpenGL:
    default:
        s_Graphics = std::make_shared<OpenGL_Backend>();
        break;
    }
}

static void CreateWindowingBackend()
{
    switch(Settings::Engine::WindowingBackend)
    {
    case gBackendIDs::wGLFW:
    default:
        s_Windowing = std::make_shared<GLFW_Backend>();
        break;
    }
}

bool BackendManager::Init()
{
    CreateGraphicsBackend();
    CreateWindowingBackend();

    if(!s_Windowing->Init() || !s_Graphics->Init())
        { return false; }
    if(!s_Windowing->CompatibleWith(s_Graphics->GetID()))
    {
        PRINT_ERROR("BackendManager::Init - The selected graphics & windowing backends are not compatible with each-other! (currently, the only existing backends are OpenGL and GLFW, so you should NOT see this message)")
        return false;
    }
    return true;
}

void BackendManager::Shutdown()
{
    s_Windowing->Shutdown();
    s_Graphics->Shutdown();

    if(m_IsImguiInitialized)
        { ImGui::DestroyContext(); }

    m_IsImguiInitialized = false;
}

std::shared_ptr<GraphicsBackend> BackendManager::Graphics()
{ return s_Graphics; }

std::shared_ptr<WindowingBackend> BackendManager::Windowing()
{ return s_Windowing; }

bool BackendManager::InitImGui()
{
    if(m_IsImguiInitialized)
        { return true; }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    if(!s_Windowing->InitImGui() || !s_Graphics->InitImGui())
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

    s_Graphics->ImGuiNewFrame();
    s_Windowing->ImGuiNewFrame();
    ImGui::NewFrame();
#   pragma message("Find a better way of disabling mouse input for ImGui when the cursor is disabled")
    if(s_Windowing->GetMouseMode() == MouseMode::Disabled)
        { ImGui::GetIO().ClearInputMouse(); }
}

void BackendManager::ImGuiRender()
{
    if(!m_IsImguiInitialized)
        { return; }

    ImGui::Render();
    s_Graphics->ImGuiRender();
}
