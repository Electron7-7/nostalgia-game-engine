#ifndef EVENT_H
#define EVENT_H

#include "common/time.hpp"
#include "console/command.hpp"
#include <string>

struct Event
{
public:
    Event(): _console_command(Commands::_NULL) {}
    Event(const ConsoleCommand& Command, const double& CreationTime = CurrentTime()): _creation_time(CreationTime), _console_command(Command) {}
    Event(const Event& CopyFrom): _creation_time(CopyFrom._creation_time), _console_command(CopyFrom._console_command) {}

    double GetCreationTime() const { return _creation_time; }
    const ConsoleCommand& GetCommand() const { return _console_command; }
    std::string EventLog() const { return std::string("EventLog - Creation time: " + std::to_string(_creation_time) + ", ConsoleCommand: \"" + _console_command.GetCommand() + "\""); }

    constexpr bool operator==(const Event& other) const
    {
        return
        (
            _console_command == other._console_command &&
            _creation_time   == other._creation_time
        );
    }
    constexpr bool operator!=(const Event& other) const { return !(*this == other); }

private:
    double _creation_time = CurrentTime();
    ConsoleCommand _console_command;
};

#endif // EVENT_H
