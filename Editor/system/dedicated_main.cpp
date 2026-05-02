#include "./program_arguments.hpp"
#include "app/nostalgia_goggles.hpp"
#define GETARGS_IMPLEMENTATION
#define GETARGS_HANDLE_INVALID_ARGS
#include "thirdparty/getargs/getargs.hpp"
#include <Nostalgia/application/application.hpp>
#include <Nostalgia/settings/engine.hpp>

#ifdef _WIN32
#   include <windows.h>
#   include <winbase.h>
#   include <shellapi.h>
#endif // _WIN32

int DedicatedMain(int argc, char** argv)
{
    getargs::set_valid_args("--help", "-h",
        "--version", "-v",
        "--no-colors",
        "--no-editor-hint",
        "--debug-thing-factory", "-d");

    if(getargs::get_args(argc, argv))
        { return 1; }

    std::string program_name{"NostalgiaGoggles"};

    if(getargs::get_flag("help") or getargs::get_flag("h"))
    {
        std::println("{}", GetHelpMessage(program_name.data()));
        return 0;
    }
    else if(getargs::get_flag("version") or getargs::get_flag("v"))
    {
        std::println("{}", GetVersionMessage(program_name.data()));
        return 0;
    }
    if(getargs::get_flag("no-colors"))
    {
        for(int i{0}; i < 7; ++i)
            { __all_labels_for_debugging[i]->enable_ansi_sequence = false; }
    }
    NostalgiaGoggles::m_sEnableThingFactoryDebugMsgs =
        getargs::get_flag("debug-thing-factory") or getargs::get_flag("d");

    Settings::Engine::IsEditorHint = not getargs::get_flag("no-editor-hint");

    NostalgiaGoggles application{};
    return Application()->Main();
}

// System-specific main functions
#ifdef _WIN32
int main(int argc, char* argv[])
{
    int windows_return = DedicatedMain(argc, argv);
    return windows_return;
}
#else // end of _WIN32, start of LINUX
int main(int argc, char* argv[])
{
    int linux_return = DedicatedMain(argc, argv);
    return linux_return;
}
#endif // end of LINUX
