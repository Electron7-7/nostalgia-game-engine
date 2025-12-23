#include "app/nostalgia_goggles.hpp"
#include "application/application.hpp"

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#include <shellapi.h>
#endif // _WIN32

int DedicatedMain(int argc, char** argv)
{
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
