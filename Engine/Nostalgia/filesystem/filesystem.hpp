#ifndef FILESYSTEM_H
#define FILESYSTEM_H

namespace FileSystem
{
    std::string GetProgramDirectory();
    std::string GetCurrentDirectory();

    Error try_WriteFileFromString(Sarg Path, Sarg Data);
    bool try_ReadFileToString(Sarg Path, std::string& Output);
    bool try_GetFileSize(Sarg Path, size_t& Output);

    // If `Path` is not empty and does not end with `/`, returns `Path + "/"`
    std::string Directory(Sarg Path);

    bool Exists(Sarg Path);
    bool IsFile(Sarg Path);
    bool IsDirectory(Sarg Path);

    bool IsAbsolute(Sarg Path);
    std::string GetAbsolute(Sarg Path);
    bool GetAbsolute(Sarg Path, std::string& Output);

    bool IsRelative(Sarg Path);
    std::string GetRelative(Sarg Path);
    bool GetRelative(Sarg Path, std::string& Output);

    std::string GetDir(Sarg Path, bool MakeAbsolute = false);
    void GetDir(Sarg Path, std::string& Output, bool MakeAbsolute = false);

    bool HasStem(Sarg Path);
    std::string GetStem(Sarg Path, bool RemoveExtension = false);
    void GetStem(Sarg Path, std::string& Output, bool RemoveExtension = false);

    bool HasExtension(Sarg Path);
    std::string GetExtension(Sarg FileName, bool RemovePeriodPrefix = false);
    void GetExtension(Sarg FileName, std::string& Output, bool RemovePeriodPrefix = false);

    void ReplaceExtension(Sarg NewExtension, std::string& Output);
    std::string ReplaceExtension(Sarg NewExtension, Sarg Path);
};

#endif // FILESYSTEM_H
