#include "render_manager.hpp"
#include "theatre_manager.hpp"
#include "ui_manager.hpp"
#include "backend_manager.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

using namespace ManagerEnums;

static RenderManager sRenderManager;
RenderManager* g_pRenderManager = &sRenderManager;


bool RenderManager::Init()
{ print_debug("RenderManager::Init"); return true; }

void RenderManager::Shutdown()
{}

ManagerEnums::TheatreReturnValue_t RenderManager::TheatreInit(bool is_first_call)
{ return FINISHED; }

ManagerEnums::TheatreReturnValue_t RenderManager::TheatreShutdown(bool is_first_call)
{ return FINISHED; }

void RenderManager::Update()
{
    // if(!g_pBackendManager->ImGuiNewFrame())
    //     { g_pBackendManager->Windowing()->ClearBuffer(glm::vec4(0.29f, 0.34f, 0.26f, 1.0f)); }
    // g_pUIManager->DrawUI();
    // if(GetTheatreState() == IN_LEVEL)
    //     { g_pBackendManager->Graphics()->RenderStoredCommands(); }
    // g_pBackendManager->ImGuiRender();
    // g_pBackendManager->Windowing()->SwapBuffers();
}
