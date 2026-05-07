#include <fstream>
#include <filesystem>

// TODO: Implement support for symlinks once I understand how to properly interact with them via std::filesystem

namespace fs = std::filesystem; // Fuck you, I'm not writing allat bullshit errytime

// I have to define this function before the code below it to avoid conflicts with MinGW's `GetCurrentDirectory` macro
std::string FileSystem::GetCurrentDirectory()
{ return fs::current_path().string(); }

// `FileSystem::GetProgramDirectory` is defined differently for Linux & Windows
#ifdef _WIN32
#   define PATH_MAX_CHARS 13107
    std::string FileSystem::GetProgramDirectory()
    {
        char _out_path[PATH_MAX_CHARS]{};
        if(GetModuleFileName(nullptr, _out_path, PATH_MAX_CHARS) == 0)
        {
            print_error("Failed to get executable location via GetModuleFileName");
            return std::string{""};
        }
         return fs::path{std::string{_out_path}}.remove_filename().string() + "/";
    }
#else  // !_WIN32
    std::string FileSystem::GetProgramDirectory()
    { return fs::read_symlink({"/proc/self/exe"}).remove_filename().string() + "/"; }
#endif // _WIN32

SafeReturn<std::string> FileSystem::Lazy::Write(Sarg inPath, uchar* inData, int inSize, OverwriteAction inAction,
    bool inDryRun)
{
    std::string _path{inPath};
    bool _exists{Exists(inPath)};
    if(_exists and inAction == CANCEL)
    {
        print_warning("A file/directory already exists at {} and will not be overwritten", inPath);
        return (IsDirectory(inPath)) ? ERR_DIR_EXISTS : ERR_FILE_EXISTS;
    }
    else if(_exists and inAction == RENAME)
        { MakeUniquePath(_path); }
    if(not inDryRun)
    {
        if(Error _status{FileSystem::Write(_path, inData, inSize)}; not _status)
            { return _status; }
    }
    return SafeReturn(_path);
}

SafeReturn<std::string> FileSystem::Lazy::Write(Sarg inPath, Sarg inData, OverwriteAction inAction,
    bool inDryRun)
{
    std::string _path{inPath};
    bool _exists{Exists(inPath)};
    if(_exists and inAction == CANCEL)
    {
        print_warning("A file/directory already exists at {} and will not be overwritten", inPath);
        return (IsDirectory(inPath)) ? ERR_DIR_EXISTS : ERR_FILE_EXISTS;
    }
    else if(_exists and inAction == RENAME)
        { MakeUniquePath(_path); }
    if(not inDryRun)
    {
        if(Error _status{FileSystem::Write(_path, inData)}; not _status)
            { return _status; }
    }
    return SafeReturn(_path);
}

bool FileSystem::MakeUniquePath(std::string& ioPath)
{
    uint i{0};
    std::string _directory{FileSystem::GetDir(ioPath, true)},
        _stem{FileSystem::GetStem(ioPath, true)},
        _extension{FileSystem::GetExtension(ioPath)};
    while(Exists(ioPath))
        { ioPath = std::format("{}/{}_{:02}{}", _directory, _stem, ++i, _extension); }
    return i > 0;
}

std::string FileSystem::MakeUniquePath(Sarg inPath)
{
    std::string _path{inPath};
    MakeUniquePath(_path);
    return _path;
}

Error FileSystem::Write(Sarg inPath, uchar* inData, int inSize)
{
    std::basic_ofstream<uchar> file(inPath);
    if(not file.is_open() or file.bad() or file.fail())
        { return ERR_FILE_OPEN; }
    file.write(inData, inSize);
    file.close();
    if(file.bad() or file.fail())
        { return ERR_FILE_WRITE; }
    return OK;
}

Error FileSystem::Write(Sarg inPath, Sarg inData)
{
    std::ofstream file(inPath);
    if(not file.is_open())
        { return ERR_FILE_WRITE; }
    file.write(inData.data(), inData.size());
    file.close();
    return OK;
}

bool FileSystem::Read(Sarg inPath, std::vector<uchar>& outData)
{
    std::ifstream file(inPath, std::ios::binary);
    if(not file.is_open())
        { return false; }
    outData = std::vector<uchar>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    file.close();
    return not file.bad() and not file.fail();
}

bool FileSystem::Read(Sarg inPath, std::string& outData)
{
    std::ifstream file(inPath);
    if(not file.is_open())
        { return false; }
    outData = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    file.close();
    return not file.bad() and not file.fail();
}

bool FileSystem::SizeOf(Sarg inPath, size_t& outSize)
{
    // https://stackoverflow.com/a/13394183
    std::ifstream _file(inPath, std::ios::binary | std::ios::ate);
    if(_file.fail() or _file.bad())
        { return false; }
    outSize = _file.tellg();
    return true;
}

bool FileSystem::Exists(Sarg string_path)
{ return fs::exists({string_path}); }

bool FileSystem::IsFile(Sarg string_path)
{ return fs::is_regular_file({string_path}); }

bool FileSystem::IsDirectory(Sarg string_path)
{ return fs::is_directory({string_path}); }

bool FileSystem::IsAbsolute(Sarg string_path)
{ return fs::path(string_path).is_absolute(); }

std::string FileSystem::GetAbsolute(Sarg string_path)
{ return fs::absolute({string_path}).string(); }

bool FileSystem::GetAbsolute(Sarg string_path, std::string& output)
{
    output = GetAbsolute(string_path);
    return fs::exists({output});
}

bool FileSystem::IsRelative(Sarg string_path)
{ return fs::path(string_path).is_relative(); }

std::string FileSystem::GetRelative(Sarg string_path)
{ return fs::relative({string_path}).string(); }

bool FileSystem::GetRelative(Sarg string_path, std::string& output)
{
    output = GetRelative(string_path);
    return fs::exists({output});
}

std::string FileSystem::GetDir(Sarg string_path, bool make_absolute)
{
    fs::path path = (make_absolute)
        ? fs::path{GetAbsolute(string_path)}
        : fs::path{string_path};
    return (path.has_filename())
        ? path.parent_path().string() + "/"
        : string_path + "/";
}

void FileSystem::GetDir(Sarg string_path, std::string& output, bool make_absolute)
{ output = GetDir(string_path, make_absolute); }

// NOTE: Should I add a boolean to toggle accepting stems with no extensions as a valid directory? Because this is basically a way of detecting whether or not a path is to a directory or not, regardless of whether or not it exists
bool FileSystem::HasStem(Sarg string_path)
{ return !(fs::path{string_path}.stem().string().empty()); }

std::string FileSystem::GetStem(Sarg string_path, bool remove_extension)
{
    if(!remove_extension)
        { return fs::path{string_path}.stem().string(); }
    return fs::path{string_path}.stem().replace_extension("").string();
}

bool FileSystem::HasExtension(Sarg string_path)
{ return !GetExtension(string_path).empty(); }

void FileSystem::GetStem(Sarg string_path, std::string& output, bool remove_extension)
{ output = GetStem(string_path, remove_extension); }

void FileSystem::GetExtension(Sarg name, std::string& output, bool remove_prefix)
{ output = GetExtension(name, remove_prefix); }

std::string FileSystem::GetExtension(Sarg name, bool remove_prefix)
{
    std::string extension = fs::path(name).extension().string();
    return (remove_prefix && extension.size() > 1) ? extension.substr(1) : extension;
}

void FileSystem::ReplaceExtension(Sarg extension, std::string& output)
{ output = fs::path{output}.replace_extension({extension}).generic_string(); }

std::string FileSystem::ReplaceExtension(Sarg extension, Sarg string_path)
{ return fs::path{string_path}.replace_extension({extension}).generic_string(); }
