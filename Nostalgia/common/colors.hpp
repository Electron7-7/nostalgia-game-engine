#ifndef COLORS_H
#define COLORS_H

// Style
namespace Sty
{
    std::string Reset = "\x1b[0m"; // Use this to reset everything back to normal
    std::string Bold  = "\x1b[1m";
    std::string Dim   = "\x1b[2m";
    std::string Norm  = "\x1b[22m";
}

// Foreground
namespace Fg
{
    std::string Black   = "\x1b[30m";
    std::string Red     = "\x1b[31m";
    std::string Green   = "\x1b[32m";
    std::string Yellow  = "\x1b[33m";
    std::string Blue    = "\x1b[34m";
    std::string Magenta = "\x1b[35m";
    std::string Cyan    = "\x1b[36m";
    std::string White   = "\x1b[37m";
    std::string Default = "\x1b[39m";
}

// Background
namespace Bg
{
    std::string Black   = "\x1b[40m";
    std::string Red     = "\x1b[41m";
    std::string Green   = "\x1b[42m";
    std::string Yellow  = "\x1b[43m";
    std::string Blue    = "\x1b[44m";
    std::string Magenta = "\x1b[45m";
    std::string Cyan    = "\x1b[46m";
    std::string White   = "\x1b[47m";
    std::string Default = "\x1b[49m";
}

#endif // COLORS_H
