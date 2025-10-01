#include "theatre_manager.hpp"
#include "theatre/theatre_data.hpp"
#include "theatre/theatre_parser.hpp"
#include "rendering/render_command.hpp"
#include "things/things.hpp"
#include "things/thing.hpp"
#include "things/actors/actor.hpp"
#include "things/actors/nostalgia_player.hpp"
#include "things/actors/light.hpp"
#include "embedded/images.hpp"
#include "embedded/models.hpp"
#include "filesystem/file_data.hpp"
#include "managers/backend_manager.hpp"
#include "colors.hpp"
#ifdef DEBUGGING
#   include "time.hpp"
#   include "testing_app/ui/imgui_debugger.hpp"
#endif // DEBUGGING

#include <vector>
#include <glm/glm.hpp>

using namespace ManagerEnums;

static TheatreManager s_TheatreManager;
TheatreManager* g_pTheatreManager = &s_TheatreManager;

std::map<id_t, std::shared_ptr<Thing>> TheatreManager::s_Things = {};

static std::shared_ptr<NostalgiaPlayer> s_LocalPlayer = std::make_shared<NostalgiaPlayer>();
static std::vector<RenderCommand> s_RenderCommandQueue = {};
static bool s_ReadyToRender = false;

void TheatreManager::Update()
{
#ifdef DEBUGGING
    if(g_PrintFrameNumbers)
        { std::print("{}TheatreManager::Update #{} @ {}\n", sty::Bold + fg::Green, Manager::m_sFrameNumber, Time::Elapsed()); }
#endif // DEBUGGING
    s_LocalPlayer->Update();
    for(auto& [id, thing] : s_Things)
        { thing->Update(); }
}

void TheatreManager::Tick()
{
#ifdef DEBUGGING
    if(g_PrintTickNumbers)
        { std::print("{}TheatreManager::Tick #{} @ {}\n", sty::Bold + fg::Blue, Manager::m_sTickNumber, Time::Elapsed()); }
#endif // DEBUGGING
    s_LocalPlayer->Tick();
    for(auto& [id, thing] : s_Things)
        { thing->Tick(); }
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

    CreateThings();
    g_pBackendManager->Graphics()->CreateRenderingData();

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
    DestroyThings();
    g_pBackendManager->Graphics()->DestroyRenderingData();

#ifdef DEBUGGING
    g_pDebugger->StopTheatreTiming(false);
#endif // DEBUGGING

    return FINISHED;
}

void TheatreManager::RenderWorld()
{
    if(!s_ReadyToRender)
        { return; }

    light_t::ClearCounts();

    for(auto& [id, thing] : s_Things)
    {
        auto actor = dynamic_pointer_cast<Actor>(thing);
        auto light = dynamic_pointer_cast<light_t>(thing);
        unsigned int shader = Shaders::BlinnPhong;
        if(light && light->m_Enabled && light->IncrementIndex())
        {
            g_pBackendManager->Graphics()->BufferLight(light.get(), shader);
            shader = Shaders::Fullbright;
        }
        if(actor && actor->m_Visible)
            { s_RenderCommandQueue.emplace_back(actor, shader); }
    }
    for(auto rendercmd = s_RenderCommandQueue.begin() ; rendercmd != s_RenderCommandQueue.end() ;)
    {
        g_pBackendManager->Graphics()->RenderSingleCommand(*rendercmd);
        rendercmd = s_RenderCommandQueue.erase(rendercmd);
    }
}

void TheatreManager::DelegateInputEvent(const InputEvent& event)
{
    s_LocalPlayer->Input(event);
    for(const auto& [id, thing] : s_Things)
        { thing->Input(event); }
}

id_t TheatreManager::CreateThing(const data_t& cData)
{
    std::shared_ptr<Thing> thing;
    data_t data = cData;

    if(s_Things.contains(data.GetID()))
    {
        PRINT_WARNING("TheatreManager::CreateThing - A Thing with ID#{} already exists; a new ID will be generated and a duplicate Thing will be made", data.GetID())
        if(!ID::AddID(data.GetID()))
            { std::print("{}[NOTE] ID#{} wasn't in the 's_ExistingIDs' set, which is what caused the previous warning (it has just been added to avoid future issues){}", sty::Bold + fg::Cyan, data.GetID(), sty::Reset); }
        data.SetID(ID::GetNewID());
    }

    if(data.GetType() == Type::NostalgiaPlayer)
    {
        thing = static_pointer_cast<Thing>(s_LocalPlayer);
        thing->SetupVariables(data_t::PlayerDefaults);
    }
    else
        { thing = s_Things[data.GetID()] = g_MakeThing(data.GetType())(); }

    thing->SetupVariables(data);
    return thing->GetID();
}

std::shared_ptr<Thing> TheatreManager::GetThing(id_t id)
{
    if(!s_Things.contains(id))
        { return std::make_shared<Thing>(); }
    return s_Things.at(id);
}

std::shared_ptr<NostalgiaPlayer> TheatreManager::GetLocalPlayer()
{ return s_LocalPlayer; }

bool TheatreManager::DestroyThing(id_t id)
{
    if(!s_Things.contains(id))
    {
        PRINT_WARNING("TheatreManager::DestroyThing - No Thing with ID#{} exists", id)
        return false;
    }
    else if(id == s_LocalPlayer->GetID())
    {
        PRINT_WARNING("TheatreManager::DestroyThing - Cannot destroy the NostalgiaPlayer Thing! (ID#{})", s_LocalPlayer->GetID())
        PRINT_DEBUG("Player ID: {}, Thing ID: {}", s_LocalPlayer->GetID(), id)
        return false;
    }

    if(!ID::Contains(id))
        { PRINT_WARNING("TheatreManager::DestroyThing - ID#{} is not in 's_ExistingIDs', but is somehow in 's_Things'! The Thing will still be destroyed, but this is highly unusual behaviour!", id) }
    return s_DestroyThing(id);
}

void TheatreManager::CreateThings()
{
    s_ReadyToRender = false;

    const std::vector<data_t>& theatre_data = TheatreParser::GetTheatreData().GetData();
    g_pBackendManager->Graphics()->DestroyRenderingData();

    // Buffer important embedded Resources
    g_pBackendManager->Graphics()->BufferTexture({Images::Missing, std::size(Images::Missing), FileType::image_JPG}, IDs::iMissing);
    g_pBackendManager->Graphics()->BufferTexture({Images::LightDebug, std::size(Images::LightDebug), FileType::image_JPG}, IDs::iLightDebug);
    g_pBackendManager->Graphics()->BufferTexture({Images::COMP04_5, std::size(Images::COMP04_5), FileType::image_PNG}, IDs::iCOMP04_5);
    g_pBackendManager->Graphics()->BufferTexture({Images::LolBit, std::size(Images::LolBit), FileType::image_PNG}, IDs::iLolBit);
    g_pBackendManager->Graphics()->BufferMesh({Models::Error, std::size(Models::Error), FileType::model_OBJ}, IDs::mError);
    g_pBackendManager->Graphics()->BufferMesh({Models::Cube, std::size(Models::Cube), FileType::model_OBJ}, IDs::mCube);
    g_pBackendManager->Graphics()->BufferMesh({Models::Ramiel, std::size(Models::Ramiel), FileType::model_OBJ}, IDs::mRamiel);

    for(const data_t& data : theatre_data)
        { CreateThing(data); }

    s_ReadyToRender = true;
}

void TheatreManager::DestroyThings()
{ s_Things.clear(); }

bool TheatreManager::s_DestroyThing(id_t id)
{
    if(!s_Things.contains(id))
        { return false; }
    s_Things.at(id)->Destroy();
    s_Things.erase(id);
    return ID::RemoveID(id);
}
