#include "render_manager.hpp"
#include <Nostalgia/settings/graphics.hpp>
#include "theatre_manager.hpp"
#include "ui_manager.hpp"
#include <Nostalgia/rendering/renderer_api.hpp>
#define GLM_ENABLE_EXPERIMENTAL

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
    if(mCanClearWindow and mRendererAPI)
    {
        mRendererAPI->SetClearColor(Settings::Graphics::ClearColor.glm());
        mRendererAPI->Clear();
    }
    g_pTheatreManager->DrawCurrentTheatre();
    g_pUIManager->DrawUI();
}

void RenderManager::SetAutomaticWindowClear(bool inEnableClear)
{ mCanClearWindow = inEnableClear; }

Farg<Unique<RendererAPI>> RenderManager::GetAPI() const
{ return mRendererAPI; }
