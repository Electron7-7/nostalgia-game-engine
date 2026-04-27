#include "render_manager.hpp"
#include "settings/graphics.hpp"
#include "theatre_manager.hpp"
#include "ui_manager.hpp"
#include "things/resource_database.hpp"
#include "rendering/texture_buffer.hpp"
#include "rendering/environment.hpp"

using namespace ManagerEnums;

static RenderManager sRenderManager;
RenderManager* g_pRenderManager{&sRenderManager};

bool RenderManager::Init()
{
    PRINT_PRETTY_FUNCTION;
    RendererAPI::Get()->Init();
    EnumRegistry::Assign(TextureType::TEXTURE_TYPE_2D,             "2DTexture");
    EnumRegistry::Assign(TextureType::TEXTURE_TYPE_CUBE,           "CubeMapTexture");
    EnumRegistry::Assign(Environment::BG_CUSTOM_COLOR,             "CustomColor");
    EnumRegistry::Assign(Environment::BG_CLEAR_COLOR,              "ClearColor");
    EnumRegistry::Assign(Environment::BG_SKYBOX,                   "Skybox");
    EnumRegistry::Assign(SAMPLER_FILTER_NONE,                      "SamplerNoFilter");
    EnumRegistry::Assign(SAMPLER_FILTER_LINEAR,                    "SamplerFilterLinear");
    EnumRegistry::Assign(SAMPLER_FILTER_NEAREST,                   "SamplerFilterNearest");
    EnumRegistry::Assign(SAMPLER_REPEAT_MODE_REPEAT,               "SamplerRepeat");
    EnumRegistry::Assign(SAMPLER_REPEAT_MODE_MIRRORED_REPEAT,      "SamplerRepeatMirrored");
    EnumRegistry::Assign(SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE,        "SamplerClampEdge");
    EnumRegistry::Assign(SAMPLER_REPEAT_MODE_CLAMP_TO_BORDER,      "SamplerClampBorder");
    EnumRegistry::Assign(SAMPLER_REPEAT_MODE_MIRROR_CLAMP_TO_EDGE, "SamplerMirrorClampEdge");
    return true;
}

void RenderManager::Shutdown()
{
    ResourceDatabase::DestroyAll();
    RendererAPI::Get()->Shutdown();
}

ManagerEnums::TheatreReturnValue_t RenderManager::TheatreInit(bool is_first_call)
{ return FINISHED; }

ManagerEnums::TheatreReturnValue_t RenderManager::TheatreShutdown(bool is_first_call)
{ return FINISHED; }

void RenderManager::Update()
{
    auto _start_time{Runtime::Current()};
    if(mCanClearWindow)
    {
        RendererAPI::Get()->SetClearColor(Settings::Graphics::ClearColor);
        RendererAPI::Get()->Clear();
    }
    g_pTheatreManager->DrawCurrentTheatre();
    if(mCanCalculateFrametime)
        { mTheatreFrametime = Runtime::Current() - _start_time; }
    g_pUIManager->DrawUI();
    if(mCanCalculateFrametime)
        { mUIFrametime = Runtime::Current() - _start_time; }
}

void RenderManager::CalculateFrameTime(bool inCalculate)
{ mCanCalculateFrametime = inCalculate; }

bool RenderManager::CalculatingFrameTime() const
{ return mCanCalculateFrametime; }

double RenderManager::GetTheatreFrameTime() const
{ return mTheatreFrametime; }

double RenderManager::GetUIFrameTime() const
{ return mUIFrametime; }

void RenderManager::SetAutomaticWindowClear(bool inEnableClear)
{ mCanClearWindow = inEnableClear; }
