#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <Nostalgia/Nostalgia.hpp>

constexpr const char* _Help_Format_String{
R"(Usage: {} [<Theatre File>] [--help] [--version] [-w|--width=<width>] [-h|--height=<width>]
             [--no-color]
    Options:
            --help           print help document
            --version        print program version
        -w, --width=WIDTH    starting width of the main window
        -h, --height=HEIGHT  starting height of the main window
            --no-color       disable ANSI colors in terminal output
)"};

constexpr const char* _Version_Format_String{"{} v{}"};

inline std::string GetHelpMessage(const char* inProgramName)
{ return std::format(_Help_Format_String, inProgramName); }

inline std::string GetVersionMessage(const char* inProgramName)
{
    return std::format(_Version_Format_String,
        inProgramName,
        NOSTALGIA_VERSION_STRING);
}

#endif // ARGUMENTS_H
