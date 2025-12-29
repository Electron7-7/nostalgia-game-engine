#include "render_manager.hpp"
#include "theatre_manager.hpp"
#include "ui_manager.hpp"
#include "core/printing.hpp"
#include "events/event.hpp"
#include "application/application.hpp"
#include "rendering/renderer_api.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

using namespace ManagerEnums;

static RenderManager sRenderManager;
RenderManager* g_pRenderManager{&sRenderManager};

bool RenderManager::Init()
{
    PRINT_PRETTY_FUNCTION;
    mRendererAPI = RendererAPI::Activate();
    return mRendererAPI->Init();
}

void RenderManager::Shutdown()
{ mRendererAPI->Shutdown(); }

ManagerEnums::TheatreReturnValue_t RenderManager::TheatreInit(bool is_first_call)
{ return FINISHED; }

ManagerEnums::TheatreReturnValue_t RenderManager::TheatreShutdown(bool is_first_call)
{ return FINISHED; }

void RenderManager::Update()
{
    if(mCanClearWindow && mRendererAPI)
    {
        mRendererAPI->SetClearColor({0.29f, 0.34f, 0.26f, 1.0f});
        mRendererAPI->Clear();
    }
    g_pTheatreManager->DrawTheatre();
    g_pUIManager->DrawUI();
}

void RenderManager::SetAutomaticWindowClear(bool inEnableClear)
{ mCanClearWindow = inEnableClear; }

Farg<Unique<RendererAPI>> RenderManager::GetAPI() const
{ return mRendererAPI; }
