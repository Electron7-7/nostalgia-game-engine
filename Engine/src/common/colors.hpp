#ifndef COLORS_H
#define COLORS_H

#include <string>

// Style
namespace Sty
{
    constexpr std::string Reset = "\x1b[0m"; // Use this to reset everything back to normal
    constexpr std::string Bold  = "\x1b[1m";
    constexpr std::string Dim   = "\x1b[2m";
    constexpr std::string Norm  = "\x1b[22m";
}

// Foreground
namespace Fg
{
    constexpr std::string Black   = "\x1b[30m";
    constexpr std::string Red     = "\x1b[31m";
    constexpr std::string Green   = "\x1b[32m";
    constexpr std::string Yellow  = "\x1b[33m";
    constexpr std::string Blue    = "\x1b[34m";
    constexpr std::string Magenta = "\x1b[35m";
    constexpr std::string Cyan    = "\x1b[36m";
    constexpr std::string White   = "\x1b[37m";
    constexpr std::string Default = "\x1b[39m";
}

// Background
namespace Bg
{
    constexpr std::string Black   = "\x1b[40m";
    constexpr std::string Red     = "\x1b[41m";
    constexpr std::string Green   = "\x1b[42m";
    constexpr std::string Yellow  = "\x1b[43m";
    constexpr std::string Blue    = "\x1b[44m";
    constexpr std::string Magenta = "\x1b[45m";
    constexpr std::string Cyan    = "\x1b[46m";
    constexpr std::string White   = "\x1b[47m";
    constexpr std::string Default = "\x1b[49m";
}

#endif // COLORS_H
