#include "app/nostalgia_goggles.hpp"
#include "application/application.hpp"
#include "managers/manager.hpp"
#include "core/printing.hpp"

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#include <shellapi.h>
#endif // _WIN32

NostalgiaGoggles nostalgia_goggles{};

int DedicatedMain(int argc, char** argv)
{
    print_app_debug("DedicatedMain - Initializing Managers");
    IManager::InitAllManagers();
    print_app_debug("DedicatedMain - Running Application");
    int return_value = g_pApplication->Main();
    print_app_debug("DedicatedMain - Shutting Down All Managers");
    IManager::ShutdownAllManagers();
    print_app_debug("DedicatedMain - Removing All Managers");
    IManager::RemoveAll();
    print_app_debug("DedicatedMain - Exiting (status:{})", return_value);
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
    print_app_debug("WinMain - Exiting");
    return win_return;
}
#else // end of _WIN32, start of LINUX
int main(int argc, char* argv[])
{
    int linux_return = DedicatedMain(argc, argv);
    print_app_debug("main - Exiting");
    return linux_return;
}
#endif // end of LINUX
