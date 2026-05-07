#ifndef FILESYSTEM_H
#define FILESYSTEM_H

namespace FileSystem
{
    enum OverwriteAction : int
    { OVERWRITE, RENAME, CANCEL };

    std::string GetProgramDirectory();
    std::string GetCurrentDirectory();

    namespace Lazy
    {
        SafeReturn<std::string> Write(Sarg inPath, uchar* inData, int inSize, OverwriteAction inAction = RENAME,
            bool inDryRun = false);
        SafeReturn<std::string> Write(Sarg inPath, Sarg inData, OverwriteAction inAction = RENAME,
            bool inDryRun = false);
    }

    bool MakeUniquePath(std::string& ioPath);
    std::string MakeUniquePath(Sarg inPath);
    Error Write(Sarg inPath, uchar* inData, int inSize);
    Error Write(Sarg inPath, Sarg inData);
    /*
    * If the given path is invalid, `ResolveFilePath` runs down a list of common starting directories and checks
    * if appending the path to the directories results in a valid filepath. If it does, the given path is updated
    * to the new path; if it doesn't, `ERR_INVALID_PATH` is returned and the given path remains untouched.
    */
    Error ResolveFilePath(std::string& ioPath);
    bool Read(Sarg inPath, std::vector<uchar>& outData);
    bool Read(Sarg inPath, std::string& outData);
    bool SizeOf(Sarg inPath, size_t& outSize);
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
