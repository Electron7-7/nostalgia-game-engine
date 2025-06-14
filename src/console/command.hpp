#ifndef CONSOLE_COMMAND_H
#define CONSOLE_COMMAND_H

#include <cstdint>
#include <string>

struct ConsoleCommand
{
public:
    // In the constructor, the UID is an 8-bit unsigned integer while the actual '_uid' variable is a normal integer.
    // This is done so that the default value of '-1' is impossible to set directly, which helps with error checking.
    constexpr ConsoleCommand(const uint8_t UID, const char* Command): _command(Command), _uid(UID) {}
    constexpr ConsoleCommand(): ConsoleCommand(0, "null") {}
    constexpr ConsoleCommand(const ConsoleCommand& CopyFrom): _command(CopyFrom._command), _uid(CopyFrom._uid) {}

    constexpr std::string Command() const { return _command; }
    constexpr int UID() const { return _uid; }

    constexpr bool operator==(const ConsoleCommand& other) const { return  (_uid  == other._uid); }
    constexpr bool operator!=(const ConsoleCommand& other) const { return !(*this == other);      }

    constexpr operator int() const { return _uid; }

private:
    const char* _command = "how_the_fuck_did_you_get_this_holy_shit_you_fucked_up_big_time";
    int _uid = -1;
};

namespace Commands
{
    constexpr ConsoleCommand _NULL               (0, "null");
    constexpr ConsoleCommand START_MOVE_FORWARD  (1, "+forward");
    constexpr ConsoleCommand STOP_MOVE_FORWARD   (2, "-forward");
    constexpr ConsoleCommand START_MOVE_BACKWARD (3, "+backward");
    constexpr ConsoleCommand STOP_MOVE_BACKWARD  (4, "-backward");
    constexpr ConsoleCommand START_STRAFE_LEFT   (5, "+strafe_left");
    constexpr ConsoleCommand STOP_STRAFE_LEFT    (6, "-strafe_left");
    constexpr ConsoleCommand START_STRAFE_RIGHT  (7, "+strafe_right");
    constexpr ConsoleCommand STOP_STRAFE_RIGHT   (8, "-strafe_right");
    constexpr ConsoleCommand START_JUMP          (9, "+jump");
    constexpr ConsoleCommand STOP_JUMP           (10, "-jump");
}

#endif // CONSOLE_COMMAND_H
