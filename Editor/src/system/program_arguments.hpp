#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <getargs/argument.hpp>

namespace Flags
{
    inline Flag Help     {"--help", "-h"};
    inline Flag Version  {"--version", "-v"};
    inline Flag NoColors {"--no-colors"};
    inline Flag DisableEditorHint {"--no-editor-hint"};
}

namespace Options
{}

constexpr const char* _Help_Printout =
R"(    Usage: Nostalgia_Goggles [-h|--help] [-v|--version] [--no-colors] [--no-editor-hint]
    Options:
        -h, --help           print help document
        -v, --version        print program version
            --no-colors      disable printout colors
            --no-editor-hint disable `Engine::IsEditorHint` (this will cause the engine to treat the editor as if it were a game)
)";

constexpr const char* _Version_Printout = "Nostalgia_Goggles v0.1-alpha";

#endif // ARGUMENTS_H
