#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>

#include "safe_return.hpp"

struct FileSystem
{
    static std::string GetProgramDirectory();
    static std::string GetCurrentDirectory();

    static SafeStatus try_WriteFileFromString(const std::string& Path, const std::string& Data);
    static bool try_ReadFileToString(const std::string& Path, std::string& Output);
    static bool try_GetFileSize(const std::string& Path, size_t& Output);

    // If `Path` is not empty and does not end with `/`, returns `Path + "/"`
    static std::string Directory(const std::string& Path);

    static bool Exists(const std::string& Path);
    static bool IsFile(const std::string& Path);
    static bool IsDirectory(const std::string& Path);

    static bool IsAbsolute(const std::string& Path);
    static std::string GetAbsolute(const std::string& Path);
    static bool GetAbsolute(const std::string& Path, std::string& Output);

    static bool IsRelative(const std::string& Path);
    static std::string GetRelative(const std::string& Path);
    static bool GetRelative(const std::string& Path, std::string& Output);

    static std::string GetDir(const std::string& Path);
    static void GetDir(const std::string& Path, std::string& Output);

    static std::string GetStem(const std::string& Path, bool RemoveExtension = false);
    static void GetStem(const std::string& Path, std::string& Output, bool RemoveExtension = false);

    static std::string GetExtension(const std::string& FileName, bool RemovePeriodPrefix = false);
    static void GetExtension(const std::string& FileName, std::string& Output, bool RemovePeriodPrefix = false);
};

#endif // FILESYSTEM_H
