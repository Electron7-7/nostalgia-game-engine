#include "./theatre_manager.hpp"
#include "theatre/theatre.hpp"
#include "theatre/theatre_file.hpp"
#include "theatre/thing_factory.hpp"

using namespace ManagerEnums;

static TheatreManager sTheatreManager;
TheatreManager* g_pTheatreManager{&sTheatreManager};

bool TheatreManager::Init()
{
    PRINT_PRETTY_FUNCTION;
    mCurrentTheatre = MakeUnique<Theatre>(MakeShared<TheatreFile::TheatreData>());
    return ThingFactory::Init();
}

void TheatreManager::Update()
{
    if(GetTheatreState() != IN_LEVEL)
        { return; }
    LockGuard<RMutex> lock{mTheatreMutex};
    mCurrentTheatre->Update();
}

void TheatreManager::Tick()
{
    if(GetTheatreState() != IN_LEVEL)
        { return; }
    LockGuard<RMutex> lock{mTheatreMutex};
    mCurrentTheatre->Tick();
}

void TheatreManager::Input(InputEvent* inInput)
{
    if(GetTheatreState() != IN_LEVEL)
        { return; }
    LockGuard<RMutex> lock{mTheatreMutex};
    mCurrentTheatre->Input(inInput);
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreInit(bool is_first_call)
{
    LockGuard<RMutex> lock{mTheatreMutex};
    if(!is_first_call)
        { return FINISHED; }
    else if(!mCurrentTheatre or !mCurrentTheatre->InitStatus())
        { return FUCKED; }
    else if(!mCurrentTheatre->Startup())
        { print_warning("Theatre::Startup failed!"); }
    return FINISHED;
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreShutdown(bool is_first_call)
{
    LockGuard<RMutex> lock{mTheatreMutex};
    if(!is_first_call)
        { return FINISHED; }
    else if(!mCurrentTheatre)
        { return FUCKED; }
    else if(!mCurrentTheatre->Shutdown())
        { print_warning("Theatre::Shutdown failed!"); }
    return FINISHED;
}

Theatre* TheatreManager::CurrentTheatre()
{ return mCurrentTheatre.get(); }

const Theatre* TheatreManager::CurrentTheatre() const
{ return mCurrentTheatre.get(); }

void TheatreManager::DrawCurrentTheatre()
{
    if(GetTheatreState() != IN_LEVEL)
        { return; }
    LockGuard<RMutex> lock{mTheatreMutex};
    mCurrentTheatre->Draw();
}

void TheatreManager::LoadNewTheatre(Unique<Theatre> inTheatre)
{
    LockGuard<RMutex> lock{mTheatreMutex};
    mCurrentTheatre = std::move(inTheatre);
    StartNewTheatre();
}

bool TheatreManager::LoadNewTheatre(Farg<FileData> inData)
{
    LockGuard<RMutex> lock{mTheatreMutex};
    mCurrentTheatre = MakeUnique<Theatre>(inData);
    if(!print_error_enum(mCurrentTheatre->InitStatus()))
    {
        return print_error("Failed to create Theatre from FileData:\n{}",
            inData.DataString());
    }
    StartNewTheatre();
    return true;
}

bool TheatreManager::LoadNewTheatre(Sarg inPath)
{
    LockGuard<RMutex> lock{mTheatreMutex};
    if(!mCurrentTheatre->Shutdown())
        { print_warning("Theatre::Shutdown failed!"); }
    mCurrentTheatre = MakeUnique<Theatre>(inPath);
    if(!print_error_enum(mCurrentTheatre->InitStatus()))
        { return print_error("Failed to create Theatre from file at '{}'", inPath); }
    StartNewTheatre();
    return true;
}
