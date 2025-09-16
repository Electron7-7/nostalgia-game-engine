#include "filesystem.hpp"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem; // Fuck you, I'm not writing allat bullshit errytime

#ifdef COMPILING_WINDOWS
#   include <windows.h>
#   include <libloaderapi.h>
    // FIXME: Improve this (it's from GraphX)
    std::string Filesystem::GetBinaryPath()
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
#else  // !COMPILING_WINDOWS
    std::string Filesystem::GetBinaryPath()
    { return std::filesystem::read_symlink(std::filesystem::path("/proc/self/exe")).remove_filename().string(); }
#endif // COMPILING_WINDOWS

bool Filesystem::PathExists(const std::string& path)
    { return(fs::exists(fs::path(path))); }

bool Filesystem::IsFile(const std::string& path) // FIXME: 'is_regular_file' could fail on some files
    { return(PathExists(path) && fs::is_regular_file(fs::path(path))); }

bool Filesystem::IsDirectory(const std::string& path)
    { return(PathExists(path) && fs::is_directory(fs::path(path))); }

std::string Filesystem::GetExtension(const std::string& name)
{
    fs::path file_name(name);
    if(!file_name.has_extension())
        { return ""; }
    return file_name.extension().string();
}

void Filesystem::MakePathAbsolute(const std::string& path, std::string& output)
{
    if(!PathExists(path) || IsDirectory(path))
        { return; }
    output = fs::absolute(fs::path(path)).generic_string();
}

SafeStatus Filesystem::try_ReadFileToString(const std::string& path, std::string& output)
{
    std::string file_path = "";
    MakePathAbsolute(path, file_path);

    std::ifstream file_stream(file_path);

    if(!file_stream.is_open())
        { return Status::FilesystemFILE_READ_ERROR; }

    output = std::string(std::istreambuf_iterator<char>(file_stream), std::istreambuf_iterator<char>());
    file_stream.close();

    return Status::NO_ERR;
}

SafeStatus Filesystem::try_GetFileSize(const std::string& path, size_t& output)
{
    std::string file_path = "";
    MakePathAbsolute(path, file_path);

    // The rest of this code is thanks to: https://stackoverflow.com/a/22131201
    FILE* image_file = fopen(file_path.c_str(), "r+");

    if(image_file == nullptr)
        { return Status::FilesystemFILE_READ_ERROR; }

    fseek(image_file, 0, SEEK_END);
    output = ftell(image_file); // This could overflow... too bad!
    fclose(image_file);
    return Status::NO_ERR;
}
