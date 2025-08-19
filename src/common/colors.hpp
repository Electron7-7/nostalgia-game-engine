#ifndef COLORS_H
#define COLORS_H

#include <string>

struct Style
{
    static constexpr std::string Reset = "\x1b[0m"; // Use this to reset everything back to normal
    static constexpr std::string Bold  = "\x1b[1m";
    static constexpr std::string Dim   = "\x1b[2m";
    static constexpr std::string Norm  = "\x1b[22m";
};

struct Foreground
{
    static constexpr std::string Black   = "\x1b[30m";
    static constexpr std::string Red     = "\x1b[31m";
    static constexpr std::string Green   = "\x1b[32m";
    static constexpr std::string Yellow  = "\x1b[33m";
    static constexpr std::string Blue    = "\x1b[34m";
    static constexpr std::string Magenta = "\x1b[35m";
    static constexpr std::string Cyan    = "\x1b[36m";
    static constexpr std::string White   = "\x1b[37m";
    static constexpr std::string Default = "\x1b[39m";
};

struct Background
{
    static constexpr std::string Black   = "\x1b[40m";
    static constexpr std::string Red     = "\x1b[41m";
    static constexpr std::string Green   = "\x1b[42m";
    static constexpr std::string Yellow  = "\x1b[43m";
    static constexpr std::string Blue    = "\x1b[44m";
    static constexpr std::string Magenta = "\x1b[45m";
    static constexpr std::string Cyan    = "\x1b[46m";
    static constexpr std::string White   = "\x1b[47m";
    static constexpr std::string Default = "\x1b[49m";
};

// If you really can't stand typing allat:
typedef Style sty;
typedef Foreground fg;
typedef Background bg;

#endif // COLORS_H