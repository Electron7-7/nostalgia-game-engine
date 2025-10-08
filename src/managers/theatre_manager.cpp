#include "theatre_manager.hpp"
#include "theatre_parser/theatre_data.hpp"
#include "theatre_parser/theatre_parser.hpp"
#include "rendering/render_command.hpp"
#include "things/thing.hpp"
#include "embedded/images.hpp"
#include "embedded/models.hpp"
#include "filesystem/file_data.hpp"
#include "managers/backend_manager.hpp"
#include "colors.hpp"
#ifdef DEBUGGING
#   include "time.hpp"
#   include "testing_app/ui/imgui_debugger.hpp"
#endif // DEBUGGING

// Headers included for `TheatreManager::GetThing` function template
#include "things/thinkers/thinker.hpp" // IWYU pragma: keep
#include "things/actors/nostalgia_player.hpp"
#include "things/actors/light.hpp"
#include "things/resources/resource.hpp" // IWYU pragma: keep
#include "things/resources/mesh.hpp" // IWYU pragma: keep
#include "things/resources/texture.hpp" // IWYU pragma: keep
#include "things/resources/font.hpp" // IWYU pragma: keep
#include "things/devices/material.hpp" // IWYU pragma: keep
#include "things/devices/mesh_instance.hpp" // IWYU pragma: keep

#include <vector>
#include <glm/glm.hpp>

using namespace ManagerEnums;

static TheatreManager s_TheatreManager;
TheatreManager* g_pTheatreManager = &s_TheatreManager;

std::map<ID, std::shared_ptr<Thing>> TheatreManager::m_sThings = {};

static std::shared_ptr<NostalgiaPlayer> s_LocalPlayer{std::make_shared<NostalgiaPlayer>()};
static std::vector<RenderCommand> s_RenderCommandQueue{};
static bool s_ReadyToRender{false};
static TheatreData sCurrentTheatreData{};
static std::string sCurrentTheatrePath{""};

void TheatreManager::Update()
{
#ifdef DEBUGGING
    if(g_PrintFrameNumbers)
        { std::print("{}TheatreManager::Update #{} @ {}\n", sty::Bold + fg::Green, Manager::m_sFrameNumber, Time::Elapsed()); }
#endif // DEBUGGING
    s_LocalPlayer->Update();
    for(auto& [id, thing] : m_sThings)
        { thing->Update(); }
}

void TheatreManager::Tick()
{
#ifdef DEBUGGING
    if(g_PrintTickNumbers)
        { std::print("{}TheatreManager::Tick #{} @ {}\n", sty::Bold + fg::Blue, Manager::m_sTickNumber, Time::Elapsed()); }
#endif // DEBUGGING
    s_LocalPlayer->Tick();
    for(auto& [id, thing] : m_sThings)
        { thing->Tick(); }
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreInit(bool is_first_call)
{
    if(!is_first_call)
        { return FINISHED; }

#ifdef DEBUGGING
    g_pDebugger->StartTheatreTiming(true);
#endif // DEBUGGING

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

    for(auto& [id, thing] : m_sThings)
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

void TheatreManager::LoadTheatreData(const TheatreData& data)
{
    sCurrentTheatreData = data;
    sCurrentTheatrePath = "";
    _Manager::StartNewTheatre();
}

bool TheatreManager::LoadTheatreFromMemory(const std::string& data)
{
    if(!TheatreParser::ParseTheatreFileFromMemory(data, sCurrentTheatreData))
    {
        PRINT_WARNING("TheatreManager::LoadTheatreFromFile - unable to load Theatre")
        DEBUG(std::println("{}Theatre File Data:{}\n{}", sty::Bold + fg::Cyan, sty::Reset, data);)
        return false;
    }
    sCurrentTheatrePath = "";
    _Manager::StartNewTheatre();
    return true;
}

bool TheatreManager::LoadTheatreFromFile(const std::string& path)
{
    if(!TheatreParser::ParseTheatreFile(path, sCurrentTheatreData))
    {
        PRINT_WARNING("TheatreManager::LoadTheatreFromFile - unable to load Theatre file '{}'", path)
        return false;
    }
    sCurrentTheatrePath = path;
    _Manager::StartNewTheatre();
    return true;
}

const TheatreData& TheatreManager::GetCurrentTheatreData()
{ return sCurrentTheatreData; }
const std::string& TheatreManager::GetCurrentTheatrePath()
{ return sCurrentTheatrePath; }

void TheatreManager::DelegateInputEvent(const InputEvent& event)
{
    s_LocalPlayer->Input(event);
    for(const auto& [id, thing] : m_sThings)
        { thing->Input(event); }
}

ID TheatreManager::CreateThing(const ThingData& cData)
{
    std::shared_ptr<Thing> thing;
    ThingData data = cData;

    if(m_sThings.contains(data.uid))
    {
        PRINT_WARNING("TheatreManager::CreateThing - A Thing with ID#{} already exists; a new ID will be generated and a duplicate Thing will be made", data.uid)
        if(!UniqueIDs::Contains(data.uid))
            { std::print("{}[NOTE] ID#{} wasn't generated by `UniqueIDs`, which is what caused the previous warning (it has just been added to avoid future issues){}", sty::Bold + fg::Cyan, data.uid, sty::Reset); }
        data.uid = UniqueIDs::Generate();
    }

    if(data.type() == ThingType::NostalgiaPlayer)
    {
        thing = static_pointer_cast<Thing>(s_LocalPlayer);
        thing->SetupVariables(ThingData::PlayerDefaults);
    }
    else
        { thing = m_sThings[data.uid] = ThingFactory::MakeThing(data.type())(); }

    thing->SetupVariables(data);
    return thing->uid();
}

std::shared_ptr<Thing> TheatreManager::GetThing(ID id)
{
    if(!m_sThings.contains(id))
        { return std::make_shared<Thing>(); }
    return m_sThings.at(id);
}

std::shared_ptr<NostalgiaPlayer> TheatreManager::GetLocalPlayer()
{ return s_LocalPlayer; }

bool TheatreManager::DestroyThing(ID id)
{
    if(!m_sThings.contains(id))
    {
        PRINT_WARNING("TheatreManager::DestroyThing - No Thing with ID#{} exists", id)
        return false;
    }
    else if(id == s_LocalPlayer->uid())
    {
        PRINT_WARNING("TheatreManager::DestroyThing - Cannot destroy the NostalgiaPlayer Thing! (ID#{})", s_LocalPlayer->uid())
        PRINT_DEBUG("Player ID: {}, Thing ID: {}", s_LocalPlayer->uid(), id)
        return false;
    }

    if(!UniqueIDs::Contains(id))
        { PRINT_WARNING("TheatreManager::DestroyThing - ID#{} is not in 's_ExistingIDs', but is somehow in 'm_sThings'! The Thing will still be destroyed, but this is highly unusual behaviour!", id) }
    return s_DestroyThing(id);
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

    for(const ThingData& data : sCurrentTheatreData.GetData())
        { CreateThing(data); }

    s_ReadyToRender = true;
}

void TheatreManager::DestroyThings()
{
    for(const auto& [key, thing] : m_sThings)
    {
        thing->Shutdown();
        m_sThings.erase(key);
    }
}

bool TheatreManager::s_DestroyThing(ID id)
{
    if(!m_sThings.contains(id))
        { return false; }
    m_sThings.at(id)->Shutdown();
    m_sThings.erase(id);
    return UniqueIDs::Erase(id);
}

bool TheatreManager::debug_GetThingAtIndex(unsigned int index, std::shared_ptr<Thing>& output)
{
    if(m_sThings.size() <= index)
        { return false; }
    auto it = m_sThings.begin();
    for(size_t i = 0 ; i <= index ; ++i)
    { ++it; }
    output = it->second;
    return true;
}

template<ThingDerived T>
std::shared_ptr<T> TheatreManager::GetThing(ID id)
{
    auto thing = dynamic_pointer_cast<T>(TheatreManager::GetThing(id));
    if(!thing)
        { return std::make_shared<T>(); }
    return thing;
}

template std::shared_ptr<Thing> TheatreManager::GetThing<Thing>(ID);
template std::shared_ptr<Thinker> TheatreManager::GetThing<Thinker>(ID);
template std::shared_ptr<Actor> TheatreManager::GetThing<Actor>(ID);
template std::shared_ptr<PointLight> TheatreManager::GetThing<PointLight>(ID);
template std::shared_ptr<SpotLight> TheatreManager::GetThing<SpotLight>(ID);
template std::shared_ptr<DirectionalLight> TheatreManager::GetThing<DirectionalLight>(ID);
template std::shared_ptr<NostalgiaPlayer> TheatreManager::GetThing<NostalgiaPlayer>(ID);
template std::shared_ptr<Resource> TheatreManager::GetThing<Resource>(ID);
template std::shared_ptr<Mesh> TheatreManager::GetThing<Mesh>(ID);
template std::shared_ptr<Texture> TheatreManager::GetThing<Texture>(ID);
template std::shared_ptr<Font> TheatreManager::GetThing<Font>(ID);
template std::shared_ptr<Device> TheatreManager::GetThing<Device>(ID);
template std::shared_ptr<MeshInstance> TheatreManager::GetThing<MeshInstance>(ID);
template std::shared_ptr<Material> TheatreManager::GetThing<Material>(ID);
