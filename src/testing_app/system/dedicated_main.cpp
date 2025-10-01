#include "testing_app/app/nostalgia_goggles.hpp"
#include "debug.hpp"

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#include <shellapi.h>
#endif // _WIN32


int DedicatedMain(int argc, char** argv)
{
    PRINT_APP_DEBUG("DedicatedMain - Creating & Initializing Application")
    g_pApplication->Create();
    g_pApplication->PreInit();

    PRINT_APP_DEBUG("DedicatedMain - Running Application")
    int return_value = g_pApplication->Main();

    PRINT_APP_DEBUG("DedicatedMain - Shutting Down Application")
    g_pApplication->PostShutdown();

    PRINT_APP_DEBUG("DedicatedMain - Exiting ({})", return_value)
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
    PRINT_APP_DEBUG("WinMain - Exiting")
    return win_return;
}
#else // end of _WIN32, start of LINUX
int main(int argc, char* argv[])
{
    int linux_return = DedicatedMain(argc, argv);
    PRINT_APP_DEBUG("main - Exiting")
    return linux_return;
}
#endif // end of LINUX
