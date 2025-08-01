#include "command.hpp"
#include "printing.hpp"

ConsoleCommand::ConsoleCommand(const char* command, ExecutionFunction command_execution_function)
: _command(command), _function(command_execution_function)
{}

ConsoleCommand::ConsoleCommand(const ConsoleCommand& copy_from)
: _command(copy_from._command), _function(copy_from._function)
{}

ConsoleCommand::ConsoleCommand()
: ConsoleCommand("null")
{}

int ConsoleCommand::ExecuteCommand()
{
    int execute_status = _function();

    if(execute_status == ExecuteStatus::FALLBACK_FUNCTION_EXECUTED)
        PRINT_DEBUG("Command \"%s\" executed the fallback command execution function, meaning its execution function was unassigned.", _command);

    return execute_status;
}

const char* ConsoleCommand::GetCommand() const
{ return _command.c_str(); }

int ConsoleCommand::FallbackExecutionFunction()
{ return ExecuteStatus::FALLBACK_FUNCTION_EXECUTED; }


// These are the definitions of all valid console commands
const ConsoleCommand Commands::_NULL               ("null"          );
const ConsoleCommand Commands::START_MOVE_FORWARD  ("+forward"      );
const ConsoleCommand Commands::STOP_MOVE_FORWARD   ("-forward"      );
const ConsoleCommand Commands::START_MOVE_BACKWARD ("+backward"     );
const ConsoleCommand Commands::STOP_MOVE_BACKWARD  ("-backward"     );
const ConsoleCommand Commands::START_STRAFE_LEFT   ("+strafe_left"  );
const ConsoleCommand Commands::STOP_STRAFE_LEFT    ("-strafe_left"  );
const ConsoleCommand Commands::START_STRAFE_RIGHT  ("+strafe_right" );
const ConsoleCommand Commands::STOP_STRAFE_RIGHT   ("-strafe_right" );
const ConsoleCommand Commands::START_JUMP          ("+jump"         );
const ConsoleCommand Commands::STOP_JUMP           ("-jump"         );
