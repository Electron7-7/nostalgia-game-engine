#ifndef EVENT_H
#define EVENT_H

#include "time.hpp"
#include "commands/command_line.hpp"

#include <format>

struct Event
{
public:
    Event(): _console_command(CommandLine::cmd_Null) {}
    Event(Command ConsoleCommand, const double& CreationTime = Time::Current()): _creation_time(CreationTime), _console_command(ConsoleCommand) {}
    Event(const Event& CopyFrom): _creation_time(CopyFrom._creation_time), _console_command(CopyFrom._console_command) {}

    double GetCreationTime() const { return _creation_time; }
    std::string GetCommand() const { return _console_command; }
    std::string EventLog() const { return std::format("EventLog - Creation time: {}, Console command: '{}'\n", _creation_time, _console_command); }

    constexpr bool operator==(const Event& other) const
    {
        return
        (
            !_console_command.compare(other._console_command) &&
            _creation_time == other._creation_time
        );
    }
    constexpr bool operator!=(const Event& other) const { return !(*this == other); }

private:
    double _creation_time = Time::Current();
    Command _console_command = CommandLine::cmd_Null;
};

#endif // EVENT_H
