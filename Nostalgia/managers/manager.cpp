#include "manager.hpp"
#include "settings/engine.hpp"
#include "application/application.hpp"
#include <cassert>

using namespace ManagerEnums;

long IManager::m_sFrameNumber{0},
    IManager::m_sTickNumber{0};
bool IManager::m_sStopRequested{false},
    IManager::m_sIsRunning{false},
    IManager::m_sIsInitialized{false},
    IManager::m_sTheatreStartRequested{false},
    IManager::m_sTheatreShutdownRequested{false},
    IManager::m_sCountFPS{false};
double IManager::m_sCurrentTime{0.0},
    IManager::m_sLastTime{0.0},
    IManager::m_sFrameRate{0.0};
ManagerEnums::TheatreState_t IManager::theatre_state{NOT_IN_LEVEL};
std::vector<IManager*> IManager::m_sGameManagers{};

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

void IManager::InvokeInput(InputEvent* inEvent)
{
    for(size_t i{0}; i < m_sGameManagers.size(); ++i)
        { m_sGameManagers.at(i)->Input(inEvent); }
}

void IManager::InvokeEvent(IEvent* inEvent)
{
    for(size_t i{0}; i < m_sGameManagers.size(); ++i)
        { m_sGameManagers.at(i)->Event(inEvent); }
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
    m_sCurrentTime = m_sLastTime = Runtime::Current();
    double _current_tick_length{0.0}, _fps_timer{0.0};
    uint _frame_counter{0};

    while(!m_sStopRequested)
    {
        // Tickrate Calculation (From GraphX)
        m_sCurrentTime = Runtime::Current();
        _current_tick_length += (m_sCurrentTime - m_sLastTime) / Settings::Engine::TickInterval();
        while(_current_tick_length >= 1.0)
        {
            --_current_tick_length;
            InvokeMethod(&IManager::Tick);
            ++m_sTickNumber;
        }
        UpdateTheatreStateMachine();
        InvokeMethod(&IManager::Update);
        MainWindow()->Update();
        ++m_sFrameNumber;
        ++_frame_counter;

        if(m_sCountFPS)
        {
            _fps_timer += Runtime::Current() - m_sCurrentTime;
            if(_fps_timer >= 1.0)
            {
                m_sFrameRate = _frame_counter;
                _frame_counter = 0;
                _fps_timer = 0.0;
            }
        }
        m_sLastTime = m_sCurrentTime;
    }

    if(theatre_state != TheatreState_t::NOT_IN_LEVEL)
    {
        ShutdownTheatre();
        UpdateTheatreStateMachine();
    }

    m_sIsRunning = false;
}

void IManager::Stop()
{ m_sStopRequested = (m_sIsRunning); }

void IManager::FPSCounter(bool inCounter)
{
    m_sCountFPS = inCounter;
    if(not inCounter)
        { m_sFrameRate = 0.0; }
}

bool IManager::FPSCounterEnabled()
{ return m_sCountFPS; }

double IManager::GetFPS()
{ return m_sFrameRate; }

long IManager::FrameNumber()
{ return m_sFrameNumber; }

long IManager::TickNumber()
{ return m_sTickNumber; }

double IManager::FixedUpdateCurrentTime()
{ return m_sFrameNumber * Settings::Engine::TickInterval(); }

double IManager::FixedUpdateDeltaTime()
{ return Settings::Engine::TickInterval(); }

double IManager::CurrentTime()
{ return m_sCurrentTime; }

double IManager::DeltaTime()
{ return m_sCurrentTime - m_sLastTime; }

ManagerEnums::TheatreState_t IManager::GetTheatreState()
{ return theatre_state; }

void IManager::StartNewTheatre()
{ m_sTheatreShutdownRequested = m_sTheatreStartRequested = true; }

void IManager::ShutdownTheatre()
{ m_sTheatreShutdownRequested = true; }
