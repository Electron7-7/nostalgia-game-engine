#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <Nostalgia/Nostalgia.hpp>

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
        NOSTALGIA_VERSION_STRING);
}

#endif // ARGUMENTS_H
