#include "./theatre_manager.hpp"
#include "theatre/theatre.hpp"
#include "theatre/theatre_file.hpp"

#define LOCK_MUTEX LockGuard<RMutex> lock{mTheatreMutex}

using namespace ManagerEnums;

static Unique<TheatreFile::TheatreData> s_pTheatreData{nullptr};
static Unique<FileData> s_pTheatreFileData{nullptr};
static Unique<std::string> sTheatreFilePath{nullptr};

static TheatreManager sTheatreManager;
TheatreManager* g_pTheatreManager{&sTheatreManager};

Theatre* TheatreManager::Current()
{
    LOCK_MUTEX;
    return m_pCurrentTheatre.get();
}

Farg<Unique<Theatre>> TheatreManager::Current() const
{ return m_pCurrentTheatre; }

void TheatreManager::LoadFromData(Farg<TheatreFile::TheatreData> inTheatreData)
{
    LOCK_MUTEX;
    s_pTheatreData = MakeUnique<TheatreFile::TheatreData>(inTheatreData);
    s_pTheatreFileData = nullptr;
    sTheatreFilePath = nullptr;
    StartNewTheatre();
}

void TheatreManager::LoadFromFileData(Farg<FileData> inTheatreFileData)
{
    LOCK_MUTEX;
    s_pTheatreData = nullptr;
    s_pTheatreFileData = MakeUnique<FileData>(inTheatreFileData);
    sTheatreFilePath = nullptr;
    StartNewTheatre();
}

void TheatreManager::LoadFromFile(Sarg inTheatreFilePath)
{
    LOCK_MUTEX;
    if(inTheatreFilePath.empty())
        { return; }
    s_pTheatreData = nullptr;
    s_pTheatreFileData = nullptr;
    sTheatreFilePath = MakeUnique<std::string>(inTheatreFilePath);
    StartNewTheatre();
}

bool TheatreManager::Init()
{
    PRINT_PRETTY_FUNCTION;
    if(not m_pCurrentTheatre)
        { m_pCurrentTheatre = MakeUnique<Theatre>(); }
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
    if(theatre_state != IN_LEVEL)
        { return; }
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
    else if(m_spNewTheatreType)
        { m_pCurrentTheatre = std::move(m_spNewTheatreType); }

    if(s_pTheatreData)
        { m_pCurrentTheatre->LoadTheatreData(*s_pTheatreData); }
    else if(s_pTheatreFileData and not print_error_enum(m_pCurrentTheatre->LoadData(*s_pTheatreFileData)))
        { return FUCKED; }
    else if(sTheatreFilePath and not print_error_enum(m_pCurrentTheatre->LoadFile(*sTheatreFilePath)))
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
    m_pCurrentTheatre->Draw();
}
