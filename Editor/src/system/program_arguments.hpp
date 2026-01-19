#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include "getargs/argument.hpp"

namespace Flags
{
    inline Flag Help     {"--help", "-h"};
    inline Flag Version  {"--version", "-v"};
    inline Flag NoColors {"--no-colors"};
}

namespace Options
{}

constexpr const char* _Help_Printout =
R"(    Usage: Nostalgia_Goggles [-h|--help] [-v|--version] [--no-colors]
    Options:
        -h, --help          print help document
        -v, --version       print program version
            --no-colors     disable printout colors
)";

constexpr const char* _Version_Printout = "Nostalgia_Goggles v0.1-alpha";

#endif // ARGUMENTS_H
