#include "theatre_manager.hpp"
#include "physics_manager.hpp"
#include "input/demo_controller.hpp"
#include "theatre_parser/theatre_data.hpp"
#include "theatre_parser/theatre_parser.hpp"
#include "rendering/render_command.hpp"
#include "things/devices/collider.hpp"
#include "things/thing.hpp"
#include "embedded/images.hpp"
#include "embedded/models.hpp"
#include "filesystem/file_data.hpp"
#include "managers/backend_manager.hpp"
#include "things/actors/nostalgia_player.hpp"
#include "things/actors/light.hpp"
#include "common/colors.hpp"
#ifdef DEBUGGING
#   include "common/time.hpp"
#   include "testing_app/ui/imgui_debugger.hpp"
#endif // DEBUGGING

#include <vector>
#include <ranges>
#include <glm/glm.hpp>

using namespace ManagerEnums;

static TheatreManager sTheatreManager;
TheatreManager* g_pTheatreManager = &sTheatreManager;

static std::shared_ptr<NostalgiaPlayer> s_LocalPlayer{std::make_shared<NostalgiaPlayer>()};
static std::vector<RenderCommand> s_RenderCommandQueue{};
static bool s_ReadyToRender{false};
static TheatreData sCurrentTheatreData{};

bool TheatreManager::Init()
{ return g_pThingFactory->Init(); }

void TheatreManager::Update()
{
#ifdef DEBUGGING
    if(g_PrintFrameNumbers)
        { std::print("{}TheatreManager::Update #{} @ {}\n", sty::Bold + fg::Green, Manager::m_sFrameNumber, Time::Elapsed()); }
#endif // DEBUGGING
    s_LocalPlayer->Update();
    for(auto& [id, thing] : mThings)
        { thing->Update(); }
}

void TheatreManager::Tick()
{
#ifdef DEBUGGING
    if(g_PrintTickNumbers)
        { std::print("{}TheatreManager::Tick #{} @ {}\n", sty::Bold + fg::Blue, Manager::m_sTickNumber, Time::Elapsed()); }
#endif // DEBUGGING
    s_LocalPlayer->Tick();
    for(auto& [id, thing] : mThings)
        { thing->Tick(); }
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreInit(bool is_first_call)
{
    if(!is_first_call)
        { return FINISHED; }

#ifdef DEBUGGING
    g_pDebugger->StartTheatreTiming(true);
#endif // DEBUGGING

    g_pDemoController->NotifyOfTheatreChange();

    s_ReadyToRender = false;

    CreateThings();
    ReadyThings();
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

    g_pDemoController->StopRecording();
    g_pDemoController->Save();
    s_ReadyToRender = false;
    DestroyThings();
    g_pBackendManager->Graphics()->DestroyRenderingData();

#ifdef DEBUGGING
    g_pDebugger->StopTheatreTiming(false);
#endif // DEBUGGING

    return FINISHED;
}

void TheatreManager::ReadyThings()
{
    for(const auto& [id, thing] : mThings)
        { thing->Ready(); }
}

void TheatreManager::RenderWorld()
{
    if(!s_ReadyToRender)
        { return; }

    light_t::ClearCounts();

    for(auto& [id, thing] : mThings)
    {
        auto actor = dynamic_pointer_cast<Actor>(thing);
        auto light = dynamic_pointer_cast<light_t>(thing);
        unsigned int shader = Shaders::BlinnPhong;
        if(light && light->mEnabled && light->IncrementIndex())
        {
            g_pBackendManager->Graphics()->BufferLight(light.get(), shader);
            shader = Shaders::Fullbright;
        }
        if(actor && actor->mVisible)
            { s_RenderCommandQueue.emplace_back(actor, shader); }
    }
    for(auto rendercmd = s_RenderCommandQueue.begin() ; rendercmd != s_RenderCommandQueue.end() ;)
    {
        g_pBackendManager->Graphics()->RenderSingleCommand(*rendercmd);
        rendercmd = s_RenderCommandQueue.erase(rendercmd);
    }
}

bool TheatreManager::ThingExists(ID uid)
{ return mThings.contains(uid); }

ID TheatreManager::GetType(ID uid)
{ return (ThingExists(uid)) ? GetThing(uid)->type() : (ID)ID::Invalid; }

void TheatreManager::LoadTheatreData(const TheatreData& data)
{
    sCurrentTheatreData = data;
    _Manager::StartNewTheatre();
}

bool TheatreManager::LoadTheatreFromMemory(const std::string& data)
{
    if(!TheatreParser::ParseTheatreFileFromMemory(data, sCurrentTheatreData))
    {
        print_warning("TheatreManager::LoadTheatreFromFile - unable to load Theatre");
        print_debugging("{}Theatre File Data:{}\n{}", sty::Bold + fg::Cyan, sty::Reset, data);
        return false;
    }
    _Manager::StartNewTheatre();
    return true;
}

bool TheatreManager::LoadTheatreFromFile(const std::string& path)
{
    if(!TheatreParser::ParseTheatreFile(path, sCurrentTheatreData))
    {
        print_warning("TheatreManager::LoadTheatreFromFile - unable to load Theatre file '{}'", path);
        return false;
    }
    _Manager::StartNewTheatre();
    return true;
}

const TheatreData& TheatreManager::GetInitialState()
{ return sCurrentTheatreData; }

TheatreData TheatreManager::GetCurrentState()
{
    TheatreData data{sCurrentTheatreData};
    data.clear();
    for(const auto& [id, thing] : mThings)
        { data.AddData(thing->GetVariables()); }
    return data;
}

std::vector<ID> TheatreManager::GetThingIDs()
{
    auto keys = std::views::keys(mThings);
    return {keys.begin(), keys.end()};
}

void TheatreManager::DelegateInputEvent(const InputEvent& event)
{
    s_LocalPlayer->Input(event);
    for(const auto& [id, thing] : mThings)
        { thing->Input(event); }
}

ID TheatreManager::CreateThing(const ThingData& cData)
{
    std::shared_ptr<Thing> thing;
    ThingData data = cData;

    if(mThings.contains(data.uid))
    {
        print_warning("TheatreManager::CreateThing - A Thing with ID#{} already exists; a new ID will be generated and a duplicate Thing will be made", data.uid);
        if(!UniqueIDs::Contains(data.uid))
            { std::print("{}[NOTE] ID#{} wasn't generated by `UniqueIDs`, which is what caused the previous warning (it has just been added to avoid future issues){}", sty::Bold + fg::Cyan, data.uid, sty::Reset); }
        data.uid = UniqueIDs::Generate();
    }

    if(data.type() == ThingType::NostalgiaPlayer)
    {
        thing = static_pointer_cast<Thing>(s_LocalPlayer);
        thing->SetVariables(ThingData::PlayerDefaults);
    }
    else
        { thing = mThings[data.uid] = g_pThingFactory->MakeThing(data.type())(); }

    thing->SetVariables(data);
    if(auto collider = dynamic_pointer_cast<Collider>(thing))
        { g_pPhysicsManager->CreateBody(thing->uid(), collider); }
    return thing->uid();
}

std::shared_ptr<NostalgiaPlayer> TheatreManager::GetLocalPlayer()
{ return s_LocalPlayer; }

bool TheatreManager::DestroyThing(ID id)
{
    if(!mThings.contains(id))
    {
        print_warning("TheatreManager::DestroyThing - No Thing with ID#{} exists", id);
        return false;
    }
    else if(id == s_LocalPlayer->uid())
    {
        print_warning("TheatreManager::DestroyThing - Cannot destroy the NostalgiaPlayer Thing! (ID#{})", s_LocalPlayer->uid());
        print_debug("Player ID: {}, Thing ID: {}", s_LocalPlayer->uid(), id);
        return false;
    }
    if(!UniqueIDs::Contains(id))
        { print_warning("TheatreManager::DestroyThing - ID#{} is not in 's_ExistingIDs', but is somehow in 'mThings'! The Thing will still be destroyed, but this is highly unusual behaviour!", id); }
    return (mThings.erase(id) != 0);
}

void TheatreManager::CreateThings()
{
    s_ReadyToRender = false;

    // Clear rendering data and buffer important embedded Resources
    g_pBackendManager->Graphics()->DestroyRenderingData();
    g_pBackendManager->Graphics()->BufferTexture({Images::Missing, std::size(Images::Missing), FileType::image_JPG}, UniqueIDs::Reserved::i_Missing);
    g_pBackendManager->Graphics()->BufferTexture({Images::LightDebug, std::size(Images::LightDebug), FileType::image_JPG}, UniqueIDs::Reserved::i_LightDebug);
    g_pBackendManager->Graphics()->BufferTexture({Images::COMP04_5, std::size(Images::COMP04_5), FileType::image_PNG}, UniqueIDs::Reserved::i_COMP04_5);
    g_pBackendManager->Graphics()->BufferTexture({Images::LolBit, std::size(Images::LolBit), FileType::image_PNG}, UniqueIDs::Reserved::i_LolBit);
    g_pBackendManager->Graphics()->BufferMesh({Models::Error, std::size(Models::Error), FileType::model_OBJ}, UniqueIDs::Reserved::m_Error);
    g_pBackendManager->Graphics()->BufferMesh({Models::Cube, std::size(Models::Cube), FileType::model_OBJ}, UniqueIDs::Reserved::m_Cube);
    g_pBackendManager->Graphics()->BufferMesh({Models::Ramiel, std::size(Models::Ramiel), FileType::model_OBJ}, UniqueIDs::Reserved::m_Ramiel);

    sCurrentTheatreData.SetupUIDsAndPriorities();
    print_debug("Reconstructed Theatre File:");
    sCurrentTheatreData.debug_PrintData();

    for(ThingData& data : sCurrentTheatreData.things_data)
        { CreateThing(data); }

    s_ReadyToRender = true;
}

void TheatreManager::DestroyThings()
{
#pragma message("When I implement multiple Theatres, they should be able to clear their own sets of IDs")
    for(const auto& [id, thing] : mThings)
    {
        if(auto collider = dynamic_pointer_cast<Collider>(thing))
            { g_pPhysicsManager->DestroyBody(id, collider); }
    }
    UniqueIDs::Clear();
    mThings.clear();
}
