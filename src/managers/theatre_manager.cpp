#include "theatre_manager.hpp"
#include "rendering/mesh.hpp"
#include "rendering/render_command.hpp"
#include "theatre/theatre_file_parser.hpp"

#include <list>
#include <glm/glm.hpp>

using namespace ManagerEnums;

static Theatre s_CurrentTheatre;
static std::vector<RenderCommand> s_RenderCommandQueue = {};
static std::list<Thing*> s_ThingStorage = {};
static std::map<unsigned int, Mesh*> s_MeshStorage = {};

static TheatreManager s_TheatreManager;
TheatreManager* g_pTheatreManager = &s_TheatreManager;

// Console variables for setting the forward and backward speed of the camera (hl2_src/app/legion/worldmanager.cpp:24-25)

TheatreManager::TheatreManager()
{
}

TheatreManager::~TheatreManager()
{
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreInit(bool is_first_call)
{
    if(!is_first_call)
    { return FINISHED; }

    CreateThings();

    return FINISHED;
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreShutdown(bool is_first_call)
{
    if(!is_first_call)
    { return FINISHED; }

    DestroyThings();

    return FINISHED;
}

void TheatreManager::LoadTheatre(const Theatre& new_theatre)
{
    Manager::ShutdownTheatre();
    s_CurrentTheatre = new_theatre;
    Manager::StartNewTheatre();
}

bool TheatreManager::try_LoadTheatre(const std::string& theatre_file)
{
    return
    (
        SafeStatus::PrintCheck(g_pTheatreFileParser->try_LoadTheatreFile(theatre_file)) &&
        SafeStatus::PrintCheck(g_pTheatreFileParser->try_ParseTheatreFile())
    );
}

Theatre* TheatreManager::GetCurrentTheatre()
{ return &s_CurrentTheatre; }

void TheatreManager::CreateThings()
{}

void TheatreManager::DestroyThings()
{}
