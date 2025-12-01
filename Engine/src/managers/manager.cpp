#include "manager.hpp"
#include "application/application.hpp"
#include "application/window.hpp"
#include "settings/engine.hpp"
#include "core/time.hpp"
#include "core/printing.hpp"

#include <GLFW/glfw3.h>
#include <cassert>
#include <thread>

#ifdef DEBUGGING
    bool g_PrintFrameNumbers = false;
    bool g_PrintTickNumbers  = false;
#endif // DEBUGGING

using namespace ManagerEnums;

long IManager::m_sFrameNumber = 0;
long IManager::m_sTickNumber  = 0;
bool IManager::m_sStopRequested = false;
bool IManager::m_sIsRunning     = false;
bool IManager::m_sIsInitialized = false;
bool IManager::m_sTheatreStartRequested    = false;
bool IManager::m_sTheatreShutdownRequested = false;
ManagerEnums::TheatreState_t IManager::theatre_state = NOT_IN_LEVEL;
std::vector<IManager*> IManager::m_sGameManagers = {};

void IManager::Add(IManager* newManager)
{
    assert(!m_sIsRunning);
    m_sGameManagers.push_back(newManager);
}

void IManager::Remove(IManager* oldManager)
{
    assert(!m_sIsRunning);
    for(int i = 0 ; i < m_sGameManagers.size() ; ++i)
    {
        if(m_sGameManagers.at(i) == oldManager)
            { m_sGameManagers.erase(m_sGameManagers.begin() + i); }
    }
}

void IManager::RemoveAll()
{
    assert(!m_sIsRunning);
    m_sGameManagers.clear();
}

void IManager::InvokeMethod(ManagerFunc_t function)
{
    for(int i = 0 ; i < m_sGameManagers.size() ; ++i) // I found out why they use '++i'! It's because 'i++' will always create a new iterator object every time it iterates, but '++i' won't! Performance!
        { (m_sGameManagers.at(i)->*function)(); }
}

void IManager::InvokeMethodReverseOrder(ManagerFunc_t function)
{
    for(int i = m_sGameManagers.size() - 1 ; i >= 0 ; --i)
        { (m_sGameManagers.at(i)->*function)(); }
}

bool IManager::InvokeMethod(ManagerInitFunc_t function)
{
    for(int i = 0 ; i < m_sGameManagers.size() ; ++i)
    {
        if(!(m_sGameManagers.at(i)->*function)())
            { return false; }
    }
    return true;
}

ManagerEnums::TheatreReturnValue_t IManager::InvokeTheatreMethod(ManagerTheatreFunction_t function, bool is_first_call)
{
    ManagerEnums::TheatreReturnValue_t return_value = FINISHED;
    for(int i = 0 ; i < m_sGameManagers.size() ; ++i)
    {
        ManagerEnums::TheatreReturnValue_t catch_return_value = (m_sGameManagers.at(i)->*function)(is_first_call);
        if(catch_return_value == FUCKED)
            { return FUCKED; }

        if(catch_return_value == MORE_WORK)
            { return_value = MORE_WORK; }
    }
    return return_value;
}

ManagerEnums::TheatreReturnValue_t IManager::InvokeTheatreMethodReverseOrder(ManagerTheatreFunction_t function, bool is_first_call)
{
    ManagerEnums::TheatreReturnValue_t return_value = FINISHED;
    for(int i = m_sGameManagers.size() - 1 ; i >= 0 ; --i)
    {
        ManagerEnums::TheatreReturnValue_t catch_return_value = (m_sGameManagers.at(i)->*function)(is_first_call);
        if(catch_return_value == FUCKED)
            { return_value = FUCKED; }

        if((catch_return_value == MORE_WORK) && (return_value != FUCKED))
            { return_value = MORE_WORK; }
    }
    return return_value;
}

bool IManager::InitAllManagers()
{
    m_sFrameNumber = 0;
    if(!InvokeMethod(&IManager::Init))
        { return false; }
    m_sIsInitialized = true;
    return true;
}

void IManager::ShutdownAllManagers()
{
    if(!m_sIsInitialized)
        { return; }
    InvokeMethodReverseOrder(&IManager::Shutdown);
    m_sIsInitialized = false;
}

void IManager::UpdateTheatreStateMachine()
{
    bool first_theatre_shutdown_frame = false; // Naming convention taken from Valve; a bit iffy on the specifics of this variable
    if(m_sTheatreShutdownRequested)
    {
        if(theatre_state != LOADING_LEVEL)
            { m_sTheatreShutdownRequested = false; }

        if(theatre_state == IN_LEVEL)
        {
            theatre_state = SHUTTING_DOWN_LEVEL;
            first_theatre_shutdown_frame = true;
        }
    }

    // Perform theatre shutdown
    if(theatre_state == SHUTTING_DOWN_LEVEL)
    {
        print_debug("Shutting Down Theatre");
        ManagerEnums::TheatreReturnValue_t return_value = InvokeTheatreMethodReverseOrder(&IManager::TheatreShutdown, first_theatre_shutdown_frame);
        if(return_value != MORE_WORK)
            { theatre_state = NOT_IN_LEVEL; }
    }

    // Do we want to switch into the theatre startup state?
    bool first_theatre_startup_frame = false;
    if(m_sTheatreStartRequested)
    {
        if(theatre_state != SHUTTING_DOWN_LEVEL)
            { m_sTheatreStartRequested = false; }

        if(theatre_state == NOT_IN_LEVEL)
        {
            theatre_state = LOADING_LEVEL;
            first_theatre_startup_frame = true;
        }
    }

    // Perform theatre load
    if(theatre_state == LOADING_LEVEL)
    {
        print_debug("Starting Up Theatre");
        ManagerEnums::TheatreReturnValue_t return_value = InvokeTheatreMethod(&IManager::TheatreInit, first_theatre_startup_frame);
        if(return_value == FUCKED)
            { theatre_state = NOT_IN_LEVEL; }

        else if(return_value == FINISHED)
            { theatre_state = IN_LEVEL; }
    }
}

void IManager::Start()
{
    assert(!m_sIsRunning && m_sIsInitialized);

    m_sIsRunning = true;
    m_sStopRequested = false;

    std::thread tick_thread(IManager::TickLoop);

    while(!m_sStopRequested)
    {
        UpdateTheatreStateMachine();
        InvokeMethod(&IManager::Update);
        MainWindow().Update();
        ++m_sFrameNumber;
    }

    tick_thread.join();
    m_sIsRunning = false;
}

void IManager::TickLoop()
{
    // Tickrate SetupVariables (From GraphX)
    double last_time = Time::Current();
    double current_tick_length = 0.0;
    double current_time = 0.0;

    while(!m_sStopRequested)
    {
        // Tickrate Calculation (From GraphX)
        current_time = Time::Current();
        current_tick_length += (current_time - last_time) / Settings::Engine::TickInterval();
        last_time = current_time;
        while(current_tick_length >= 1.0)
        {
            --current_tick_length;
            ++m_sTickNumber;
            InvokeMethod(&IManager::Tick);
        }
    }
}

void IManager::Stop()
{ m_sStopRequested = (m_sIsRunning); }

long IManager::FrameNumber()
{ return m_sFrameNumber; }

long IManager::TickNumber()
{ return m_sTickNumber; }

ManagerEnums::TheatreState_t IManager::GetTheatreState()
{ return theatre_state; }

void IManager::StartNewTheatre()
{ m_sTheatreShutdownRequested = m_sTheatreStartRequested = true; }

void IManager::ShutdownTheatre()
{ m_sTheatreShutdownRequested = true; }
