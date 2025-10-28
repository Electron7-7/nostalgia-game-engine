#include "manager.hpp"
#include "settings/engine.hpp"
#include "common/time.hpp"
#include "common/printing.hpp"

#include <GLFW/glfw3.h>
#include <cassert>
#include <thread>

#ifdef DEBUGGING
    bool g_PrintFrameNumbers = false;
    bool g_PrintTickNumbers  = false;
#endif // DEBUGGING

using namespace ManagerEnums;

long _Manager::m_sFrameNumber = 0;
long _Manager::m_sTickNumber  = 0;
bool _Manager::m_sStopRequested = false;
bool _Manager::m_sIsRunning     = false;
bool _Manager::m_sIsInitialized = false;
bool _Manager::m_sTheatreStartRequested    = false;
bool _Manager::m_sTheatreShutdownRequested = false;
ManagerEnums::TheatreState_t _Manager::theatre_state = NOT_IN_LEVEL;
std::vector<_Manager*> _Manager::m_sGameManagers = {};

void _Manager::Add(_Manager* new_manager)
{
    assert(!m_sIsRunning);
    m_sGameManagers.push_back(new_manager);
}

void _Manager::Remove(_Manager* old_manager)
{
    assert(!m_sIsRunning);
    for(int i = 0 ; i < m_sGameManagers.size() ; ++i)
    {
        if(m_sGameManagers.at(i) == old_manager)
            { m_sGameManagers.erase(m_sGameManagers.begin() + i); }
    }
}

void _Manager::RemoveAll()
{
    assert(!m_sIsRunning);
    m_sGameManagers.clear();
}

void _Manager::InvokeMethod(ManagerFunc_t function)
{
    for(int i = 0 ; i < m_sGameManagers.size() ; ++i) // I found out why they use '++i'! It's because 'i++' will always create a new iterator object every time it iterates, but '++i' won't! Performance!
        { (m_sGameManagers.at(i)->*function)(); }
}

void _Manager::InvokeMethodReverseOrder(ManagerFunc_t function)
{
    for(int i = m_sGameManagers.size() - 1 ; i >= 0 ; --i)
        { (m_sGameManagers.at(i)->*function)(); }
}

bool _Manager::InvokeMethod(ManagerInitFunc_t function)
{
    for(int i = 0 ; i < m_sGameManagers.size() ; ++i)
    {
        print_debug("Initializing {}", m_sGameManagers.at(i)->DebugName());
        if(!(m_sGameManagers.at(i)->*function)())
            { return false; }
    }
    return true;
}

ManagerEnums::TheatreReturnValue_t _Manager::InvokeTheatreMethod(ManagerTheatreFunction_t function, bool is_first_call)
{
    ManagerEnums::TheatreReturnValue_t return_value = FINISHED;
    for(int i = 0 ; i < m_sGameManagers.size() ; ++i)
    {
        print_debug("Invoking Theatre method in {}", m_sGameManagers.at(i)->DebugName());
        ManagerEnums::TheatreReturnValue_t catch_return_value = (m_sGameManagers.at(i)->*function)(is_first_call);
        if(catch_return_value == FUCKED)
            { return FUCKED; }

        if(catch_return_value == MORE_WORK)
            { return_value = MORE_WORK; }
    }
    return return_value;
}

ManagerEnums::TheatreReturnValue_t _Manager::InvokeTheatreMethodReverseOrder(ManagerTheatreFunction_t function, bool is_first_call)
{
    ManagerEnums::TheatreReturnValue_t return_value = FINISHED;
    for(int i = m_sGameManagers.size() - 1 ; i >= 0 ; --i)
    {
        print_debug("(reverse) Invoking Theatre method in {}", m_sGameManagers.at(i)->DebugName());
        ManagerEnums::TheatreReturnValue_t catch_return_value = (m_sGameManagers.at(i)->*function)(is_first_call);
        if(catch_return_value == FUCKED)
            { return_value = FUCKED; }

        if((catch_return_value == MORE_WORK) && (return_value != FUCKED))
            { return_value = MORE_WORK; }
    }
    return return_value;
}

bool _Manager::InitAllManagers()
{
    m_sFrameNumber = 0;
    if(!InvokeMethod(&_Manager::Init))
        { return false; }
    m_sIsInitialized = true;
    return true;
}

void _Manager::ShutdownAllManagers()
{
    if(!m_sIsInitialized)
        { return; }
    InvokeMethodReverseOrder(&_Manager::Shutdown);
    m_sIsInitialized = false;
}

void _Manager::UpdateTheatreStateMachine()
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
        ManagerEnums::TheatreReturnValue_t return_value = InvokeTheatreMethodReverseOrder(&_Manager::TheatreShutdown, first_theatre_shutdown_frame);
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
        ManagerEnums::TheatreReturnValue_t return_value = InvokeTheatreMethod(&_Manager::TheatreInit, first_theatre_startup_frame);
        if(return_value == FUCKED)
            { theatre_state = NOT_IN_LEVEL; }

        else if(return_value == FINISHED)
            { theatre_state = IN_LEVEL; }
    }
}

void _Manager::Start()
{
    assert(!m_sIsRunning && m_sIsInitialized);

    m_sIsRunning = true;
    m_sStopRequested = false;

    std::thread tick_thread(_Manager::TickLoop);

    while(!m_sStopRequested)
    {
        UpdateTheatreStateMachine();
        InvokeMethod(&_Manager::Update);
        ++m_sFrameNumber;
    }

    tick_thread.join();
    m_sIsRunning = false;
}

void _Manager::TickLoop()
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
            InvokeMethod(&_Manager::Tick);
        }
    }
}

void _Manager::Stop()
{ m_sStopRequested = (m_sIsRunning); }

long _Manager::FrameNumber()
{ return m_sFrameNumber; }

long _Manager::TickNumber()
{ return m_sTickNumber; }

ManagerEnums::TheatreState_t _Manager::GetTheatreState()
{ return theatre_state; }

void _Manager::StartNewTheatre()
{ m_sTheatreShutdownRequested = m_sTheatreStartRequested = true; }

void _Manager::ShutdownTheatre()
{ m_sTheatreShutdownRequested = true; }
