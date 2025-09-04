#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "safe_return.hpp"
#include "binary_file_data.hpp"

struct Filesystem
{
    static bool PathExists(const std::string& Path);
    static bool IsFile(const std::string& File);
    static bool IsDirectory(const std::string& Directory);
    static std::string GetBinaryPath();
    static std::string GetExtension(const std::string& FileName);

    static void MakePathAbsolute(const std::string& Filepath, std::string& Output);
    static SafeStatus try_ReadFileToString(const std::string& Filepath, std::string& Output);
    static SafeStatus try_ReadFileToUCharArray(const std::string& Filepath, BinaryFileData& Output);
};

#endif // FILESYSTEM_H
