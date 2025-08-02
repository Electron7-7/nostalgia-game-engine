#include "command_line.hpp"
#include "printing.hpp"

bool CommandLine::is_processing_commands = false;
std::vector<ConsoleCommand> CommandLine::_commands = {};
ConsoleCommand CommandLine::_current_command_in_queue = Commands::_NULL;

void CommandLine::BeginProcessingCommands()
{
    if(is_processing_commands)
        return;

    is_processing_commands = true;
}

void CommandLine::EndProcessingCommands()
{
    if(!is_processing_commands)
        return;

    is_processing_commands = false;
}

bool CommandLine::DequeueNextCommand()
{
    if(_commands.empty())
        return false;

    _current_command_in_queue = _commands.at(0);
    _commands.erase(_commands.cbegin());

    return true;
}

ConsoleCommand& CommandLine::GetCommandInQueue()
{ return _current_command_in_queue; }

bool CommandLine::AddCommandToQueue(ConsoleCommand command)
{
    if(is_processing_commands)
        return false;

    _commands.insert(_commands.end(), command);
    return true;
}

int CommandLine::RunCommandInQueue()
{ return(RunCommand(_current_command_in_queue)); }

int CommandLine::RunCommand(ConsoleCommand& command)
{
    if(Commands::IsValid(command))
    {
        PRINT_WARNING("CommandLine::RunCommand - Command \"{}\" is invalid.", command.GetCommand())
        return 1;
    }

    if(command == Commands::_NULL)
    {
        PRINT_WARNING("CommandLine::RunCommand - Command \"{}\" is NULL!", command.GetCommand())
        return 1;
    }

    PRINT_DEBUG("Attempting to execute command \"{}\".", command.GetCommand())
    // NOTE: Maybe change this to return '0' if 'command.ExecuteCommand' returns 'ExecuteStatus::FALLBACK_FUNCTION_EXECUTED'
    return(command.ExecuteCommand());
}
