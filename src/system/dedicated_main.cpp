#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#include <shellapi.h>
#endif // _WIN32

#include "debugging.hpp"
#include "app/toymaker.hpp"

int DedicatedMain(int argc, char* argv[])
{
    // TODO: Implement a class that handles apps & implements a function called "Run" that will then execute "TheToyMaker::Main" at some point
    global_App->Create();
    global_App->PreInit();

    int return_value = global_App->Main();

    global_App->Shutdown();
    global_App->PostShutdown();

    return return_value;
}

// System-specific main functions
#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int argc,win_return = -1;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    win_return = DedicatedMain(argc, (char**)argv);
    GlobalFree(argv);
    return win_return;
}
#else // end of _WIN32, start of LINUX
int main(int argc, char* argv[])
{
    int linux_return = DedicatedMain(argc, argv);
    return linux_return;
}
#endif // end of LINUX