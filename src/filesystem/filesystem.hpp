#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "safe_return.hpp"

struct Filesystem
{
    static bool PathExists(const std::string& Path);
    static bool IsFile(const std::string& File);
    static bool IsDirectory(const std::string& Directory);
    static std::string GetBinaryPath();
    static std::string GetExtension(const std::string& FileName);

    static void MakePathAbsolute(const std::string& Filepath, std::string& Output);
    static SafeStatus try_ReadFileToString(const std::string& Filepath, std::string& Output);
    static SafeStatus try_GetFileSize(const std::string& Filepath, size_t& Output);
};

#endif // FILESYSTEM_H
