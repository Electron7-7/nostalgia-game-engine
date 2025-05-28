// System-specific includes
#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#include <shellapi.h>
#elif POSIX
#endif

int DedicatedMain(int argc, char** argv); // Defined in 'dedicated/sys_dedicated.cpp'

// System-specific main functions
#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int argc,win_return = -1;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    win_return = main(argc, (char**)argv);
    GlobalFree(argv);
    return win_return;
}
#else
int main(int argc, char* argv[])
{
    int linux_return = DedicatedMain(argc, argv);
    return linux_return;
}
#endif