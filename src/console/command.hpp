#ifndef CONSOLE_COMMAND_H
#define CONSOLE_COMMAND_H

#include <string>
#include <set>

struct ConsoleCommand
{
public:
    typedef int (*ExecutionFunction)();

    ConsoleCommand(const char* Command, ExecutionFunction CommandExecutionFunction = &ConsoleCommand::FallbackExecutionFunction);
    ConsoleCommand(const ConsoleCommand& CopyFrom);
    ConsoleCommand();

    int ExecuteCommand();
    const char* GetCommand() const;
    int GetUID() const;

    constexpr bool operator==(const ConsoleCommand& other) const { return !(_command.compare(other._command)); }
    constexpr bool operator!=(const ConsoleCommand& other) const { return !(*this == other); }

    struct ExecuteStatus
    {
        static constexpr int NO_ERROR                   = 0;
        static constexpr int ERROR                      = 1;
        static constexpr int FALLBACK_FUNCTION_EXECUTED = 2;
    };

private:
    static int FallbackExecutionFunction();

    ExecutionFunction _function = &ConsoleCommand::FallbackExecutionFunction;

    std::string _command = "how_the_fuck_did_you_get_this_holy_shit_you_fucked_up_big_time";
};

struct Commands
{
    static const ConsoleCommand _NULL;
    static const ConsoleCommand START_MOVE_FORWARD;
    static const ConsoleCommand STOP_MOVE_FORWARD;
    static const ConsoleCommand START_MOVE_BACKWARD;
    static const ConsoleCommand STOP_MOVE_BACKWARD;
    static const ConsoleCommand START_STRAFE_LEFT;
    static const ConsoleCommand STOP_STRAFE_LEFT;
    static const ConsoleCommand START_STRAFE_RIGHT;
    static const ConsoleCommand STOP_STRAFE_RIGHT;
    static const ConsoleCommand START_JUMP;
    static const ConsoleCommand STOP_JUMP;

    // Terrible, horrible, no good, very bad way of checking that a command is valid
    static bool IsValid(ConsoleCommand& TestMe)
    {
        return
        (
            TestMe == _NULL               ||
            TestMe == START_MOVE_FORWARD  ||
            TestMe == STOP_MOVE_FORWARD   ||
            TestMe == START_MOVE_BACKWARD ||
            TestMe == STOP_MOVE_BACKWARD  ||
            TestMe == START_STRAFE_LEFT   ||
            TestMe == STOP_STRAFE_LEFT    ||
            TestMe == START_STRAFE_RIGHT  ||
            TestMe == STOP_STRAFE_RIGHT   ||
            TestMe == START_JUMP          ||
            TestMe == STOP_JUMP
        );
    }
};


#endif // CONSOLE_COMMAND_H
