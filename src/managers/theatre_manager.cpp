#include "theatre_manager.hpp"
#include "managers/backend_manager.hpp"
#include "rendering/backends/backend.hpp"
#include "safe_return.hpp"
#include "theatre/theatre_data.hpp"
#include "theatre/theatre_parser.hpp"
#include "rendering/render_command.hpp"
#include "things/actors/actor.hpp"
#include "things/resources/basic/texture.hpp"
#include "things/things.hpp"
#include "things/actors/nostalgia_player.hpp"
#include "things/resources/basic/mesh.hpp"
#ifdef DEBUGGING
#   include "colors.hpp"
#   include "time.hpp"
#   include "testing_app/ui/imgui_debugger.hpp"
#endif // DEBUGGING

#include <vector>
#include <glm/glm.hpp>

using namespace ManagerEnums;

static std::shared_ptr<NostalgiaPlayer> s_LocalPlayer = std::make_shared<NostalgiaPlayer>();
static std::vector<RenderCommand> s_RenderCommandQueue = {};
static bool s_ReadyToRender = false;

static TheatreManager s_TheatreManager;
TheatreManager* g_pTheatreManager = &s_TheatreManager;

std::map<id_t, std::shared_ptr<Thing>> TheatreManager::s_Things = {};
int TheatreManager::s_PointLightsCount       = 0;
int TheatreManager::s_SpotLightsCount        = 0;
int TheatreManager::s_DirectionalLightsCount = 0;

void TheatreManager::Update()
{
#ifdef DEBUGGING
    if(g_PrintFrameNumbers)
        { std::print("{}TheatreManager::Update #{} @ {}\n", sty::Bold + fg::Green, Manager::m_sFrameNumber, Time::Elapsed()); }
#endif // DEBUGGING
}

void TheatreManager::Tick()
{
#ifdef DEBUGGING
    if(g_PrintTickNumbers)
        { std::print("{}TheatreManager::Tick #{} @ {}\n", sty::Bold + fg::Blue, Manager::m_sTickNumber, Time::Elapsed()); }
#endif // DEBUGGING
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreInit(bool is_first_call)
{
    if(!is_first_call)
        { return FINISHED; }

#ifdef DEBUGGING
    g_pDebugger->StartTheatreTiming(true);
#endif // DEBUGGING

    if(!SafeStatus::PrintCheck(TheatreParser::try_ParseTheatre()))
        { return FUCKED; }

    s_ReadyToRender = false;

    CreateObjects();
    g_pBackendManager->GetGraphicsBackend()->CreateRenderingData();

    s_ReadyToRender = true;

#ifdef DEBUGGING
    g_pDebugger->StopTheatreTiming(true);
#endif // DEBUGGING

    return FINISHED;
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreShutdown(bool is_first_call)
{
    if(!is_first_call)
        { return FINISHED; }

#ifdef DEBUGGING
    g_pDebugger->StartTheatreTiming(false);
#endif // DEBUGGING

    s_ReadyToRender = false;
    DestroyObjects();
    g_pBackendManager->GetGraphicsBackend()->DestroyRenderingData();

#ifdef DEBUGGING
    g_pDebugger->StopTheatreTiming(false);
#endif // DEBUGGING

    return FINISHED;
}

void TheatreManager::RenderWorld()
{
    if(!s_ReadyToRender)
        { return; }
    for(auto& [id, thing] : s_Things)
    {
        std::shared_ptr<Actor> actor = dynamic_pointer_cast<Actor>(thing);
        if(actor)
            { s_RenderCommandQueue.emplace_back(actor, Shaders::BlinnPhong); }
    }
    for(auto rendercmd = s_RenderCommandQueue.begin() ; rendercmd != s_RenderCommandQueue.end() ;)
    {
        g_pBackendManager->GetGraphicsBackend()->RenderSingleCommand(*rendercmd);
        rendercmd = s_RenderCommandQueue.erase(rendercmd);
    }
}

bool TheatreManager::try_DestroyThing(id_t thing_id)
{
    if(!s_Things.contains(thing_id))
        { return false; }
    s_Things.erase(thing_id);
    return true;
}

std::shared_ptr<NostalgiaPlayer> TheatreManager::GetLocalPlayer()
{ return s_LocalPlayer; }

std::shared_ptr<Thing>& TheatreManager::GetThing(id_t id)
{
    if(!s_Things.contains(id))
        { return s_Things.begin()->second; }
    return s_Things.at(id);
}

const std::shared_ptr<Thing>& TheatreManager::cGetThing(id_t id)
{
    if(!s_Things.contains(id))
        { return s_Things.cbegin()->second; }
    return s_Things.at(id);
}

int TheatreManager::PointLightsCount()
{ return s_PointLightsCount; }

int TheatreManager::SpotLightsCount()
{ return s_SpotLightsCount; }

int TheatreManager::DirectionalLightsCount()
{ return s_DirectionalLightsCount; }

void TheatreManager::CreateObjects()
{
    s_ReadyToRender = false;

    const std::vector<data_t>& theatre_data = TheatreParser::GetTheatreData().GetData();
    g_pBackendManager->GetGraphicsBackend()->DestroyRenderingData();

    // FIXME: FIND A BETTER PLACE TO DO Error/Missing SHIT
    Mesh::Error.m_Name = "Error Mesh";
    Mesh::Error.m_Type = Type::Mesh;
    Texture::Missing.m_Name = "Missing Texture";
    Texture::Missing.m_Type = Type::Texture;
    g_pBackendManager->GetGraphicsBackend()->BufferTexture(&Texture::Missing);
    g_pBackendManager->GetGraphicsBackend()->BufferMesh(&Mesh::Error);

    for(const data_t& data : theatre_data)
    {
        auto& thing = s_Things[data.GetID()] = g_MakeThing(data.GetType())();
        thing->m_ID   = data.GetID();
        thing->m_Type = data.GetType();
        thing->m_Name = data.GetName();
        thing->SetupVariables(data);

        if(data.GetType() == Type::NostalgiaPlayer)
        {
            const auto& try_CastPlayer = static_pointer_cast<NostalgiaPlayer>(thing);
            if(try_CastPlayer)
                { s_LocalPlayer = try_CastPlayer; }
        }
    }

    s_ReadyToRender = true;
}

void TheatreManager::DestroyObjects()
{ s_Things.clear(); }
