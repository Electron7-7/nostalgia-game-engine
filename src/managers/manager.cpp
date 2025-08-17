#include "manager.hpp"
#include "printing.hpp"
#include "settings/settings.hpp"

#include <GLFW/glfw3.h>
#include <cassert>
#include <thread>

int _Manager::frame_number = 0;
int _Manager::tick_number = 0;
bool _Manager::stop_requested = false;
bool _Manager::is_running = false;
bool _Manager::is_initialized = false;
bool _Manager::theatre_start_requested = false;
bool _Manager::theatre_shutdown_requested = false;
TheatreState_t _Manager::theatre_state = NOT_IN_LEVEL;

std::vector<_Manager*> _Manager::game_managers = {};

void _Manager::Add(_Manager* new_manager)
{
    assert(!is_running);
    game_managers.insert(game_managers.end(), new_manager);
}

void _Manager::Remove(_Manager* old_manager)
{
    assert(!is_running);
    for(int i = 0 ; i < game_managers.size() ; i++)
    {
        if(game_managers.at(i) == old_manager) // Possible point of error(?): I'm guessing that since I'm comparing two pointers, there might be a possibility that two pointers can be pointing to the same object, but return 0 when compared?
        { game_managers.erase(game_managers.begin() + i); } // Possible point of error: I fucked up the index and it should be "i - 1", instead
    }
}

// FIXME: This don't feel right... _Manager::Remove has all these checks and is much more delicate
void _Manager::RemoveAll()
{ game_managers.clear(); }

void _Manager::InvokeMethod(ManagerFunc_t function)
{
    for(int i = 0 ; i < game_managers.size() ; ++i) // I found out why they use '++i'! It's because 'i++' will always create a new iterator object every time it iterates, but '++i' won't! Performance!
    { (game_managers.at(i)->*function)(); }
}

void _Manager::InvokeMethodReverseOrder(ManagerFunc_t function)
{
    for(int i = game_managers.size() - 1 ; i >= 0 ; --i)
    { (game_managers.at(i)->*function)(); }
}

bool _Manager::InvokeMethod(ManagerInitFunc_t function)
{
    for(int i = 0 ; i < game_managers.size() ; ++i)
    {
        if(!(game_managers.at(i)->*function)())
        { return false; }
    }
    return true;
}

TheatreReturnValue_t _Manager::InvokeTheatreMethod(ManagerTheatreFunction_t function, bool is_first_call)
{
    TheatreReturnValue_t return_value = FINISHED;
    for(int i = 0 ; i < game_managers.size() ; ++i)
    {
        TheatreReturnValue_t catch_return_value = (game_managers.at(i)->*function)(is_first_call);
        if(catch_return_value == FUCKED)
        { return FUCKED; }

        if(catch_return_value == MORE_WORK)
        { return_value = MORE_WORK; }
    }
    return return_value;
}

TheatreReturnValue_t _Manager::InvokeTheatreMethodReverseOrder(ManagerTheatreFunction_t function, bool is_first_call)
{
    TheatreReturnValue_t return_value = FINISHED;
    for(int i = 0 ; i < game_managers.size() ; ++i)
    {
        TheatreReturnValue_t catch_return_value = (game_managers.at(i)->*function)(is_first_call);
        if(catch_return_value == FUCKED)
        { return_value = FUCKED; }

        if((catch_return_value == MORE_WORK) && (return_value != FUCKED))
        { return_value = MORE_WORK; }
    }
    return return_value;
}

bool _Manager::InitAllManagers()
{
    frame_number = 0;
    // Educated guess is that this will try and invoke the "Init" method on every game manager in the list (via the "InvokeMethod" function), and if it returns false that means we hit a snag
    if(!InvokeMethod(&_Manager::Init))
    { return false; }

    is_initialized = true;
    return true;
}

void _Manager::ShutdownAllManagers()
{
    if(is_initialized) // Todo: maybe make this an early return?
    {
        InvokeMethodReverseOrder(&_Manager::Shutdown);
        is_initialized = false;
    }
}

void _Manager::UpdateTheatreStateMachine()
{
    bool first_theatre_shutdown_frame = false; // Naming convention taken from Valve; a bit iffy on the specifics of this variable
    if(theatre_shutdown_requested)
    {
        if(theatre_state != LOADING_LEVEL)
        { theatre_shutdown_requested = false; } // Todo: make this prettier

        if(theatre_state == IN_LEVEL)
        {
            theatre_state = SHUTTING_DOWN_LEVEL;
            first_theatre_shutdown_frame = true;
        }
    }

    // Perform theatre shutdown
    if(theatre_state == SHUTTING_DOWN_LEVEL)
    {
        TheatreReturnValue_t return_value = InvokeTheatreMethodReverseOrder(&_Manager::TheatreShutdown, first_theatre_shutdown_frame);
        if(return_value != MORE_WORK)
        { theatre_state = NOT_IN_LEVEL; }
    }

    // Do we want to switch into the theatre startup state?
    bool first_theatre_startup_frame = false;
    if(theatre_start_requested)
    {
        if(theatre_state != SHUTTING_DOWN_LEVEL)
        { theatre_start_requested = false; } // Todo: make this prettier

        if(theatre_state == NOT_IN_LEVEL)
        {
            theatre_state = LOADING_LEVEL;
            first_theatre_startup_frame = true;
        }
    }

    // Perform theatre load
    if(theatre_state == LOADING_LEVEL)
    {
        TheatreReturnValue_t return_value = InvokeTheatreMethod(&_Manager::TheatreInit, first_theatre_startup_frame);
        if(return_value == FUCKED)
        { theatre_state = NOT_IN_LEVEL; } // Todo: make this prettier

        else if(return_value == FINISHED)
        { theatre_state = IN_LEVEL; }
    }
}

void _Manager::Start()
{
    assert(!is_running && is_initialized);

    is_running = true;
    stop_requested = false;

    std::thread tick_thread(_Manager::Tick);

    int number_of_managers = game_managers.size();

    while(!stop_requested)
    {
        UpdateTheatreStateMachine();

        for(int i = 0 ; i < number_of_managers ; ++i)
        {
            if(!game_managers.at(i)->PleaseTickMeInAFixedUpdateLoop())
            { game_managers.at(i)->Update(); }
        }

        ++frame_number;
    }

    tick_thread.join();
    is_running = false;
}

void _Manager::Tick()
{
    double last_time = 0.0;
    int number_of_managers = game_managers.size();

    while(!stop_requested)
    {
        float current_tick_length = 0.0f;
        auto now = std::chrono::steady_clock::now();
        auto duration = now - start_time;
        double current_time = std::chrono::duration<double>(duration).count();
        current_tick_length += (current_time - last_time) / Settings::Engine::TickInterval;
        last_time = current_time;

        while(current_tick_length >= 1.0f)
        {
            for(int i = 0 ; i < number_of_managers ; ++i)
            {
                if(game_managers.at(i)->PleaseTickMeInAFixedUpdateLoop())
                { game_managers.at(i)->Update(); }
            }
            ++tick_number;
            current_tick_length--;
        }
    }
}

void _Manager::Stop()
{ stop_requested = (is_running); PRINT_DEBUG("{}_Manager::Stop{} has been called\n", Color::Front::DefaultBold, Color::Reset) }

int _Manager::FrameNumber()
{ return frame_number; }

TheatreState_t _Manager::GetTheatreState()
{ return theatre_state; }

void _Manager::StartNewTheatre()
{
    theatre_shutdown_requested = true;
    theatre_start_requested = true;
}

void _Manager::ShutdownTheatre()
{
    theatre_shutdown_requested = true;
}
