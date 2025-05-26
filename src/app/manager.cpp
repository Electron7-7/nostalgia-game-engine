#include "manager.hpp"
#include <cassert>
#include <GLFW/glfw3.h>

int _Manager::frame_number = 0;
bool _Manager::stop_requested = false;
bool _Manager::is_running = false;
bool _Manager::is_initialized = false;
bool _Manager::level_start_requested = false;
bool _Manager::level_shutdown_requested = false;
float _Manager::current_time = 0.0f;
float _Manager::last_time = 0.0f;
LevelState_t _Manager::level_state = NOT_IN_LEVEL;

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
        if(game_managers.at(i) == old_manager) // Possible point of error(?): I'm guessing that since I'm comparing two pointers, there might be a possibility that two pointers can be pointing to the same object, but return 0 when compared?
            game_managers.erase(game_managers.begin() + i); // Possible point of error: I fucked up the index and it should be "i - 1", instead
}

void _Manager::RemoveAll()
{ game_managers.clear(); }

void _Manager::InvokeMethod(ManagerFunc_t function)
{
    for(int i = 0 ; i < game_managers.size() ; ++i) // Note: I'm assuming they use "++i" here since the "main" manager will also be in this vector, and we don't want it touching itself (ew...)
        (game_managers.at(i)->*function)();
}

void _Manager::InvokeMethodReverseOrder(ManagerFunc_t function)
{
    for(int i = game_managers.size() ; i >= 0 ; --i) // See note in _Manager::InvokeMethod
        (game_managers.at(i)->*function)();
}

bool _Manager::InvokeMethod(ManagerInitFunc_t function)
{
    for(int i = 0 ; i < game_managers.size() ; ++i) // See note in _Manager::InvokeMethod
        if(!(game_managers.at(i)->*function)())
            return false;
    return true;
}

LevelReturnValue_t _Manager::InvokeLevelMethod(ManagerLevelFunction_t function, bool is_first_call)
{
    LevelReturnValue_t return_value = FINISHED;
    for(int i = 0 ; i < game_managers.size() ; ++i)
    {
        LevelReturnValue_t catch_return_value = (game_managers.at(i)->*function)(is_first_call);
        if(catch_return_value == FUCKED)
            return FUCKED;
        if(catch_return_value == MORE_WORK)
            return_value = MORE_WORK;
    }
    return return_value;
}

LevelReturnValue_t _Manager::InvokeLevelMethodReverseOrder(ManagerLevelFunction_t function, bool is_first_call)
{
    LevelReturnValue_t return_value = FINISHED;
    for(int i = 0 ; i < game_managers.size() ; ++i)
    {
        LevelReturnValue_t catch_return_value = (game_managers.at(i)->*function)(is_first_call);
        if(catch_return_value == FUCKED)
            return_value = FUCKED;
        if((catch_return_value == MORE_WORK) && (return_value != FUCKED))
            return_value = MORE_WORK;
    }
    return return_value;
}

bool _Manager::InitAllManagers()
{
    frame_number = 0;
    // Educated guess is that this will try and invoke the "Init" method on every game manager in the list (via the "InvokeMethod" function), and if it returns false that means we hit a snag
    if(!InvokeMethod(&_Manager::Init))
        return false;

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

void _Manager::UpdateLevelStateMachine()
{
    bool first_level_shutdown_frame = false; // Naming convention taken from Valve; a bit iffy on the specifics of this variable
    if(level_shutdown_requested)
    {
        if(level_state != LOADING_LEVEL)
            level_shutdown_requested = false; // Todo: make this prettier
        if(level_state == IN_LEVEL)
        {
            level_state = SHUTTING_DOWN_LEVEL;
            first_level_shutdown_frame = true;
        }
    }

    // Perform level shutdown
    if(level_state == SHUTTING_DOWN_LEVEL)
    {
        LevelReturnValue_t return_value = InvokeLevelMethodReverseOrder(&_Manager::LevelShutdown, first_level_shutdown_frame);
        if(return_value != MORE_WORK)
            level_state = NOT_IN_LEVEL;
    }

    // Do we want to switch into the level startup state?
    bool first_level_startup_frame = false;
    if(level_start_requested)
    {
        if(level_state != SHUTTING_DOWN_LEVEL)
            level_start_requested = false; // Todo: make this prettier
        if(level_state == NOT_IN_LEVEL)
        {
            level_state = LOADING_LEVEL;
            first_level_startup_frame = true;
        }
    }

    // Perform level load
    if(level_state == LOADING_LEVEL)
    {
        LevelReturnValue_t return_value = InvokeLevelMethod(&_Manager::LevelInit, first_level_startup_frame);
        if(return_value == FUCKED)
            level_state = NOT_IN_LEVEL; // Todo: make this prettier
        else if(return_value == FINISHED)
            level_state = IN_LEVEL;
    }
}

void _Manager::Start()
{
    assert(!is_running && is_initialized);

    is_running = true;
    stop_requested = false;

    /*
    Nothing in this line of code is even close to being implemented, but it might be a nice addition in the future:
    ---------------------------------------------------------------------------------------------------------------
        // This option is useful when running the app twice on the same machine
        // It makes the 2nd instance of the app run a lot faster
        bool bPlayNice = ( CommandLine()->CheckParm( "-yieldcycles" ) != 0 );
    */

    float start_time = current_time = last_time = glfwGetTime();
    int number_of_fixed_update_frames_ticked = 0;
    int number_of_managers = game_managers.size();

    while(!stop_requested)
    {
        UpdateLevelStateMachine();

        last_time = current_time;
        current_time = glfwGetTime();
        int number_of_fixed_update_frames_needed = 1 + (int)((current_time - start_time) / TICK_INTERVAL);
        while(number_of_fixed_update_frames_needed > number_of_fixed_update_frames_ticked)
        {
            for(int i = 0 ; i < number_of_managers ; ++i)
                if(game_managers.at(i)->PleaseTickMeInAFixedUpdateLoop())
                    game_managers.at(i)->Update();
            ++frame_number;
            ++number_of_fixed_update_frames_ticked;
        }

        for(int i = 0 ; i < number_of_managers ; ++i)
            if(!game_managers.at(i)->PleaseTickMeInAFixedUpdateLoop())
                game_managers.at(i)->Update();

        /*
        See last multi-line comment
        ---------------------------
            if ( bPlayNice )
            {
                Sleep( 1 );
            }
        */
    }

    is_running = false;
}

void _Manager::Stop()
{ stop_requested = (is_running); } // Smaller way of saying: if(is_running) { stop_requested = true; }

int _Manager::FrameNumber()
{ return frame_number; }

float _Manager::FixedUpdateCurrentTime()
{ return frame_number * TICK_INTERVAL; }

float _Manager::FixedUpdateDeltaTime()
{ return TICK_INTERVAL; }

float _Manager::CurrentTime()
{ return current_time; }

float _Manager::DeltaTime()
{ return current_time - last_time; }

LevelState_t _Manager::GetLevelState()
{ return level_state; }

void _Manager::StartNewLevel()
{
    level_shutdown_requested = true;
    level_start_requested = true;
}

void _Manager::ShutdownLevel()
{
    level_shutdown_requested = true;
}