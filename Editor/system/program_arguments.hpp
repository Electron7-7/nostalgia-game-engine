#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <Nostalgia/Nostalgia.hpp>
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

constexpr const char* _Help_Format_String{
R"(Usage: {} [-h|--help] [-v|--version] [--no-colors] [--no-editor-hint]
    Options:
        -h, --help           print help document
        -v, --version        print program version
            --no-colors      disable printout colors
            --no-editor-hint sets `Engine::IsEditorHint` to false
)"};

constexpr const char* _Version_Format_String{"{} v{}"};

inline std::string GetHelpMessage(const char* inProgramName)
{ return std::format(_Help_Format_String, inProgramName); }

inline std::string GetVersionMessage(const char* inProgramName)
{
    return std::format(_Version_Format_String,
        inProgramName,
        NOSTALGIA_VERSION_FULL);
}

#endif // ARGUMENTS_H
