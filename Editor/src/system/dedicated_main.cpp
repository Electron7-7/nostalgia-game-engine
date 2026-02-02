#include "./program_arguments.hpp"
#include "app/nostalgia_goggles.hpp"
#include <getargs/argument_parser.hpp>
#include <Nostalgia/application/application.hpp>
#include <Nostalgia/settings/engine.hpp>

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#include <shellapi.h>
#endif // _WIN32

int DedicatedMain(int argc, char** argv)
{
    global_ArgumentParser->AddFlag(&Flags::Help);
    global_ArgumentParser->AddFlag(&Flags::Version);
    global_ArgumentParser->AddFlag(&Flags::NoColors);
    global_ArgumentParser->AddFlag(&Flags::DisableEditorHint);

    int parser_status{global_ArgumentParser->ParseArguments(argc, argv)};
    if(parser_status == ARG_STATUS_FAILED)
        { return 1; }

    if(Flags::Help.IsActive())
    {
        std::println("{}\n\t{}",
            _Help_Printout,
            _Version_Printout);
        return 0;
    }
    else if(Flags::Version.IsActive())
    {
        std::println("\t{}\n", _Version_Printout);
        return 0;
    }
    else if(Flags::NoColors.IsActive())
    {
        for(int i{0}; i < 7; ++i)
            { __all_labels_for_debugging[i]->enable_ansi_sequence = false; }
    }

    Settings::Engine::IsEditorHint = !Flags::DisableEditorHint.IsActive();

    NostalgiaGoggles application{};
    return Application()->Main();
}

// System-specific main functions
#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int argc,windows_return = -1;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    windows_return = DedicatedMain(argc, (char**)argv);
    GlobalFree(argv);
    return windows_return;
}
#else // end of _WIN32, start of LINUX
int main(int argc, char* argv[])
{
    int linux_return = DedicatedMain(argc, argv);
    return linux_return;
}
#endif // end of LINUX
