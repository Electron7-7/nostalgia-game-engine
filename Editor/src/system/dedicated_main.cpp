#include "app/nostalgia_goggles.hpp"
#include "application/application.hpp"
#include "managers/manager.hpp"
#include "core/printing.hpp"

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#include <shellapi.h>
#endif // _WIN32

static NostalgiaGoggles sNostalgiaGoggles{};

int DedicatedMain(int argc, char** argv)
{
    print_app("Initializing Managers");
    IManager::InitAllManagers();
    print_app("Running Application");
    int return_value = Application()->Main();
    print_app("Shutting Down All Managers");
    IManager::ShutdownAllManagers();
    print_app("Removing All Managers");
    IManager::RemoveAll();
    return return_value;
}

// System-specific main functions
#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    print_app("WinMain Start (Windows main)");
    int argc,windows_return = -1;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    print_app("Entering DedicatedMain");
    windows_return = DedicatedMain(argc, (char**)argv);
    GlobalFree(argv);
    print_app("Exiting WinMain (status:{})", windows_return);
    return windows_return;
}
#else // end of _WIN32, start of LINUX
int main(int argc, char* argv[])
{
    print_app("main Start (Linux main)");
    print_app("Entering DedicatedMain");
    int linux_return = DedicatedMain(argc, argv);
    print_app("Exiting main (status:{})", linux_return);
    return linux_return;
}
#endif // end of LINUX
