#ifndef CONSOLE_H
#define CONSOLE_H

#include "command.hpp"

#include <vector>

struct CommandLine
{
public:
    static void BeginProcessingCommands();
    static void EndProcessingCommands();
    static bool DequeueNextCommand();
    static ConsoleCommand& GetCommandInQueue();
    static int RunCommandInQueue();
    static bool AddCommandToQueue(ConsoleCommand);

private:
    static bool is_processing_commands;

    static std::vector<ConsoleCommand> _commands;
    static ConsoleCommand _current_command_in_queue;

    static int RunCommand(ConsoleCommand&);
};

#endif // CONSOLE_H
