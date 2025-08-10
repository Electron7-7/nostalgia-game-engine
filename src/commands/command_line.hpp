#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <map>
#include <list>
#include <string>

struct SafeStatus; // Forward-declaration

typedef std::string Command;
typedef SafeStatus (*CommandFunction)();

struct CommandLine
{
public:
    static bool Init();

    static void ProcessCommands();
    static void PushFront(Command ConsoleCommand);
    static bool PopFront();

    static SafeStatus try_RunCommand(Command ConsoleCommand);

    typedef const char* ConsoleCommand; // Because std::string can't be constexpr

    static constexpr ConsoleCommand cmd_Null        = "null";
    static constexpr ConsoleCommand cmd_ExitProgram = "please don't do this unless you know what you're doing";

private:
    static std::map<Command, CommandFunction> _commands;
    static std::list<Command> _active_queue;
    static std::list<Command> _process_queue;

    static bool is_initialized;
    static bool is_copying_queue;
    static bool is_processing_commands;
    static Command next_command;

    static bool ExtractNextCommand();
};

#endif // COMMAND_LINE_H
