#include "./theatre_manager.hpp"
#include "theatre/theatre.hpp"
#include "theatre/theatre_file.hpp"
#include "settings/engine.hpp"

#define LOCK_MUTEX LockGuard<RMutex> lock{mTheatreMutex}

using namespace ManagerEnums;

static Shared<TheatreFile::TheatreData> s_pTheatreData{nullptr};
static Shared<FileData> s_pTheatreFileData{nullptr};
static std::string sTheatreFilePath{};

static TheatreManager sTheatreManager;
TheatreManager* g_pTheatreManager{&sTheatreManager};

Theatre* TheatreManager::Current()
{
    if(not m_pCurrentTheatre)
        { m_pCurrentTheatre = MakeUnique<Theatre>(); }
    return m_pCurrentTheatre.get();
}

void TheatreManager::LoadFromData(Shared<TheatreFile::TheatreData> inTheatreData)
{
    s_pTheatreData = inTheatreData;
    s_pTheatreFileData = nullptr;
    sTheatreFilePath.clear();
    StartNewTheatre();
}

void TheatreManager::LoadFromFileData(Shared<FileData> inTheatreFileData)
{
    s_pTheatreData = nullptr;
    s_pTheatreFileData = inTheatreFileData;
    sTheatreFilePath.clear();
    StartNewTheatre();
}

void TheatreManager::LoadFromFile(Sarg inTheatreFilePath)
{
    s_pTheatreData = nullptr;
    s_pTheatreFileData = nullptr;
    sTheatreFilePath = inTheatreFilePath;
    StartNewTheatre();
}

bool TheatreManager::Init()
{
    PRINT_PRETTY_FUNCTION;
    m_pCurrentTheatre = MakeUnique<Theatre>();
    return true;
}

void TheatreManager::Update()
{
    if(theatre_state != IN_LEVEL)
        { return; }
    LOCK_MUTEX;
    m_pCurrentTheatre->Update();
}

void TheatreManager::Tick()
{
    LOCK_MUTEX;
    m_pCurrentTheatre->Tick();
}

void TheatreManager::Input(InputEvent* inInput)
{
    if(theatre_state != IN_LEVEL)
        { return; }
    LOCK_MUTEX;
    m_pCurrentTheatre->Input(inInput);
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreInit(bool is_first_call)
{
    LOCK_MUTEX;
    if(!is_first_call)
        { return FINISHED; }
    else if(not m_pCurrentTheatre or not m_pCurrentTheatre->InitStatus())
        { m_pCurrentTheatre = MakeUnique<Theatre>(); }
    if(s_pTheatreData)
        { m_pCurrentTheatre->LoadTheatreData(s_pTheatreData); }
    else if(s_pTheatreFileData and not print_error_enum(m_pCurrentTheatre->LoadData(*s_pTheatreFileData)))
        { return FUCKED; }
    else if(not sTheatreFilePath.empty() and
        not print_error_enum(m_pCurrentTheatre->LoadFile(sTheatreFilePath)))
            { return FUCKED; }
    if(not m_pCurrentTheatre->Startup())
        { print_warning("Theatre::Startup failed!"); }
    return FINISHED;
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreShutdown(bool is_first_call)
{
    LOCK_MUTEX;
    if(!is_first_call)
        { return FINISHED; }
    else if(!m_pCurrentTheatre)
        { return FUCKED; }
    m_pCurrentTheatre->Shutdown();
    m_pCurrentTheatre = MakeUnique<Theatre>();
    return FINISHED;
}

void TheatreManager::DrawCurrentTheatre()
{
    if(theatre_state != IN_LEVEL or not m_pCurrentTheatre)
        { return; }
    LOCK_MUTEX;
    m_pCurrentTheatre->Draw(not Settings::Engine::IsEditorHint);
}
