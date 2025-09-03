#ifndef GRAPHX_SANITY_EXECUTABLE_LOCATOR
#define GRAPHX_SANITY_EXECUTABLE_LOCATOR

#include <string>

#ifdef COMPILING_WINDOWS

#include <windows.h>
#include <libloaderapi.h>

// FIXME: Improve this (it's from GraphX)
inline std::string getBinaryPath()
{
    char out_path[MAX_PATH] = {0};
    GetModuleFileNameA(NULL, out_path, MAX_PATH);
    std::string buffer = "";
    bool filename_removed = false;
    for(int i = (sizeof(out_path)/sizeof(out_path[0])) ; i >= 0 ; --i)
    {
        if(out_path[i] == 0x00)
            continue;
        if(((out_path[i] == '\\' && !(out_path[i+1] == ' ')) || out_path[i] == '/') && !filename_removed)
        {
            buffer = "";
            filename_removed = true;
        }

        buffer += out_path[i];
    }

    std::string new_buffer = "";

    for(int i = buffer.length() - 1 ; i > 0 ; --i)
        new_buffer += buffer[i];

    return new_buffer + "\\";
}

inline std::string BINARY_PATH(getBinaryPath());
inline std::string EXTERNAL_THEATRES_DIRECTORY("theatres");

#else  // COMPILING_WINDOWS

#include <filesystem> // IWYU pragma: keep

inline std::string BINARY_PATH(std::filesystem::read_symlink(std::filesystem::path("/proc/self/exe")).remove_filename().string());
inline std::string EXTERNAL_THEATRES_DIRECTORY("theatres");

#endif // COMPILING_WINDOWS

#endif
