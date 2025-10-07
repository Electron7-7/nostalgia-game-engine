#include "filesystem.hpp"

#include <fstream>
#include <filesystem>

#pragma message("TODO: Implement support for symlinks once I understand how to properly interact with them via std::filesystem")

namespace fs = std::filesystem; // Fuck you, I'm not writing allat bullshit errytime

// `FileSystem::GetProgramDirectory` is defined differently for Linux & Windows
#ifdef COMPILING_WINDOWS
#   include <windows.h>
#   include <libloaderapi.h>
#   pragma message("FIXME: Improve this (it's from GraphX)")
    std::string FileSystem::GetProgramDirectory()
    {
        char out_path[MAX_PATH] = {0};
        GetModuleFileNameA(NULL, out_path, MAX_PATH);
        std::string buffer = "";
        bool filename_removed = false;
        for(int i = (sizeof(out_path)/sizeof(out_path[0])) ; i >= 0 ; --i)
        {
            if(out_path[i] == 0x00)
                { continue; }
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
    std::string FileSystem::GetProgramDirectory()
    { return fs::read_symlink({"/proc/self/exe"}).remove_filename().string(); }
#endif // COMPILING_WINDOWS

std::string FileSystem::GetCurrentDirectory()
{ return fs::current_path().string(); }

SafeStatus FileSystem::try_WriteFileFromString(const std::string& path, const std::string& data)
{
    std::string absolute_path = GetAbsolute(path);
    if(!Exists(GetStem(absolute_path)))
        { return Status::FileSystemINVALID_PATH; }
    else if(FileSystem::Exists(path))
        { PRINT_WARNING("FileSystem::try_WriteFileFromString - Attempting to overwrite file at '{}'!", path) }

    std::ofstream file(absolute_path);
    if(!file.is_open())
        { return Status::FileSystemFAILED_TO_WRITE_FILE; }
    file.write(data.data(), data.size());
    file.close();
    return Status::NO_ERR;
}

bool FileSystem::try_ReadFileToString(const std::string& string_path, std::string& output)
{
    std::string file_path = "";
    if(!GetAbsolute(string_path, file_path))
        { return false; }

    std::ifstream file_stream(file_path);

    if(!file_stream.is_open())
        { return false; }

    output = std::string(std::istreambuf_iterator<char>(file_stream), std::istreambuf_iterator<char>());
    file_stream.close();

    return true;
}

bool FileSystem::try_GetFileSize(const std::string& string_path, size_t& output)
{
    std::string file_path = "";
    if(!GetAbsolute(string_path, file_path))
        { return false; }

    // The rest of this code is thanks to: https://stackoverflow.com/a/22131201
    FILE* image_file = fopen(file_path.c_str(), "r+");

    if(image_file == nullptr)
        { return false; }

    fseek(image_file, 0, SEEK_END);
    output = ftell(image_file); // This could overflow... too bad!
    fclose(image_file);
    return true;
}

std::string FileSystem::Directory(const std::string& string_path)
{
    if(string_path.empty() || string_path.ends_with('/'))
        { return string_path; }
    return string_path + "/";
}

bool FileSystem::Exists(const std::string& string_path)
{ return fs::exists({string_path}); }

bool FileSystem::IsFile(const std::string& string_path)
{ return fs::is_regular_file({string_path}); }

bool FileSystem::IsDirectory(const std::string& string_path)
{ return fs::is_directory({string_path}); }

bool FileSystem::IsAbsolute(const std::string& string_path)
{ return fs::path(string_path).is_absolute(); }

std::string FileSystem::GetAbsolute(const std::string& string_path)
{ return fs::absolute({string_path}).string(); }

bool FileSystem::GetAbsolute(const std::string& string_path, std::string& output)
{
    output = GetAbsolute(string_path);
    return fs::exists({output});
}

bool FileSystem::IsRelative(const std::string& string_path)
{ return fs::path(string_path).is_relative(); }

std::string FileSystem::GetRelative(const std::string& string_path)
{ return fs::relative({string_path}).string(); }

bool FileSystem::GetRelative(const std::string& string_path, std::string& output)
{
    output = GetRelative(string_path);
    return fs::exists({output});
}

std::string FileSystem::GetDir(const std::string& string_path)
{
    fs::path path(string_path);
    return (path.has_filename()) ? path.parent_path().string() : string_path;
}

void FileSystem::GetDir(const std::string& string_path, std::string& output)
{ output = GetDir(string_path); }

std::string FileSystem::GetStem(const std::string& string_path, bool remove_extension)
{
    if(!remove_extension)
        { return fs::path({string_path}).stem().string(); }
    return fs::path({string_path}).stem().replace_extension("").string();
}

void FileSystem::GetStem(const std::string& string_path, std::string& output, bool remove_extension)
{ output = GetStem(string_path, remove_extension); }

void FileSystem::GetExtension(const std::string& name, std::string& output, bool remove_prefix)
{ output = GetExtension(name, remove_prefix); }

std::string FileSystem::GetExtension(const std::string& name, bool remove_prefix)
{
    std::string extension = fs::path(name).extension().string();
    return (remove_prefix && extension.size() > 1) ? extension.substr(1) : extension;
}
