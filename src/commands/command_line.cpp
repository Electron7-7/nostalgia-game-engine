#include "command_line.hpp"
#include "managers/render_manager.hpp"
#include "safe_return.hpp"
#include "time.hpp"
#include "printing.hpp"
#include "managers/manager.hpp"

SafeStatus HardExitProgram();
SafeStatus PrototypeFullscreen();

std::map<Command, CommandFunction> CommandLine::_commands =
{
    { cmd_ExitProgram, &HardExitProgram },
    { cmd_PrototypeFullscreen, &PrototypeFullscreen },
};

std::list<Command> CommandLine::_active_queue = {};
std::list<Command> CommandLine::_process_queue = {};
bool CommandLine::is_initialized = false;
bool CommandLine::is_copying_queue = false;
bool CommandLine::is_processing_commands = false;
Command CommandLine::next_command = cmd_Null;


bool CommandLine::Init()
{
    if(is_initialized)
        return true;

    is_initialized = true;
    return true;
}

SafeStatus CommandLine::try_RunCommand(Command command)
{
    if(!_commands.contains(command))
        return Status::CommandLineINVALID_COMMAND;

    _commands.at(command)();
    return Status::NO_ERROR;
}

void CommandLine::ProcessCommands()
{
    if(is_processing_commands)
        return;

    is_processing_commands = true;

    is_copying_queue = true;

    _process_queue = _active_queue;

    is_copying_queue = false;

    while(ExtractNextCommand())
    {
        if(!_commands.contains(next_command))
        {
            PRINT_WARNING("CommandLine::ProcessCommands - invalid command '{}'", next_command)
            continue;
        }

        SafeStatus command_status = _commands.at(next_command)();
        if(command_status.Status() != Status::NO_ERROR)
            PRINT_WARNING("CommandLine::ProcessCommands - command returned with the status: '{}'", command_status.Printout())
    }

    is_processing_commands = false;
    _process_queue.clear(); // Just in case
}

void CommandLine::PushFront(Command command)
{
    WAIT_FOR(is_copying_queue, 1.0)

    _active_queue.emplace(_active_queue.cbegin(), command);
}

bool CommandLine::PopFront()
{
    if(_active_queue.size() == 0)
        return false;
    _active_queue.pop_front();
    return true;
}

bool CommandLine::ExtractNextCommand()
{
    if(_process_queue.size() == 0)
    {
        next_command = "";
        return false;
    }

    next_command = _process_queue.front();
    _process_queue.pop_front();
    return true;
}


SafeStatus HardExitProgram()
{
    _Manager::Stop();
    return Status::NO_ERROR;
}

SafeStatus PrototypeFullscreen()
{
    global_RenderManager->prototype_SetFullscreen(true);
    return Status::NO_ERROR;
}
