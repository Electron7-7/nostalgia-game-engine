#include "backend_manager.hpp"
#include "settings/settings.hpp"
#include "DearImGui/imgui.h"
#include "common/printing.hpp"
// Include all backends here
#include "backends/graphics/opengl.hpp"
#include "backends/windowing/glfw.hpp"

static BackendManager sBackendManager;
BackendManager* g_pBackendManager = &sBackendManager;

static std::shared_ptr<GraphicsBackend>  s_pGraphics{nullptr};
static std::shared_ptr<WindowingBackend> s_pWindowing{nullptr};

static void CreateGraphicsBackend()
{
    switch(Settings::Engine::GraphicsBackend)
    {
    case BackendIDs::gOpenGL:
    default:
        s_pGraphics = std::make_shared<OpenGL_Backend>();
        return;
    }
}

static void CreateWindowingBackend()
{
    switch(Settings::Engine::WindowingBackend)
    {
    case BackendIDs::wGLFW:
    default:
        s_pWindowing = std::make_shared<GLFW_Backend>();
        return;
    }
}

bool BackendManager::Init()
{
#pragma message("TODO: select the backends from a saved config (so you can change backends)")
    CreateGraphicsBackend();
    CreateWindowingBackend();

    if(!s_pWindowing->Init() || !s_pGraphics->Init())
        { return false; }
    else if(!s_pWindowing->CompatibleWith(s_pGraphics->GetID()))
        { return print_error("BackendManager::Init - The selected graphics & windowing backends are not compatible with each-other! (currently, the only existing backends are OpenGL and GLFW, so you should NOT see this message)"); }
    else if(!Settings::World::UpdateOrientation(s_pGraphics->GetID()))
        { print_warning("BackendManager::Init - The selected graphics backend '{}' was not able to update the world orientation; this may be the cause of any visual issues", s_pGraphics->GetID().name()); }
    return true;
}

void BackendManager::Shutdown()
{
    ShutdownImGui();
    s_pGraphics->Shutdown();
    s_pWindowing->Shutdown();
}

ManagerEnums::TheatreReturnValue_t BackendManager::TheatreInit(bool first_call)
{
    if(!first_call)
        { return ManagerEnums::FINISHED; }
    Windowing()->SetMouseMode(MouseMode::Disabled);
    return ManagerEnums::FINISHED;
}

ManagerEnums::TheatreReturnValue_t BackendManager::TheatreShutdown(bool first_call)
{
    if(!first_call)
        { return ManagerEnums::FINISHED; }
    Windowing()->SetMouseMode(MouseMode::Normal);
    return ManagerEnums::FINISHED;
}

std::shared_ptr<GraphicsBackend> BackendManager::Graphics()
{ return s_pGraphics; }

std::shared_ptr<WindowingBackend> BackendManager::Windowing()
{ return s_pWindowing; }

bool BackendManager::InitImGui()
{
    if(mImGuiInitialized)
        { return true; }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    mImGuiInitialized = s_pWindowing->InitImGui() && s_pGraphics->InitImGui();
    return mImGuiInitialized;
}

void BackendManager::ShutdownImGui()
{
    if(!mImGuiInitialized)
        { return; }
    s_pGraphics->ShutdownImGui();
    s_pWindowing->ShutdownImGui();
    ImGui::DestroyContext();
    mImGuiInitialized = false;
}

void BackendManager::ImGuiNewFrame()
{
    if(!mImGuiInitialized)
        { return; }
    s_pGraphics->ImGuiNewFrame();
    s_pWindowing->ImGuiNewFrame();
    ImGui::NewFrame();
#   pragma message("Find a better way of disabling mouse input for ImGui when the cursor is disabled")
    if(s_pWindowing->GetMouseMode() == MouseMode::Disabled)
        { ImGui::GetIO().ClearInputMouse(); }
}

void BackendManager::ImGuiRender()
{
    if(!mImGuiInitialized)
        { return; }
    ImGui::Render();
    s_pGraphics->ImGuiRender();
}
