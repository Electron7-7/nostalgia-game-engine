#include "file_data.hpp"
#include "filesystem.hpp"
#include "core/printing.hpp"

#include <map>

static std::map<std::string, FileType> s_FileTypesByExtension =
{
    { ".otf",  FileType::font_OTF  },
    { ".ttf",  FileType::font_TTF  },
    { ".png",  FileType::image_PNG },
    { ".jpg",  FileType::image_JPG },
    { ".jpeg", FileType::image_JPG },
    { ".obj",  FileType::model_OBJ },
    { ".frag", FileType::glsl_FRAG },
    { ".vert", FileType::glsl_VERT },
};

FileType FileData::s_DetectFileType(const std::string& path)
{
    std::string extension = FileSystem::GetExtension(path);
    if(!s_FileTypesByExtension.contains(extension))
        { return FileType::Unknown; }
    return s_FileTypesByExtension.at(extension);
}

FileData::FileData() = default;

FileData::FileData(const unsigned char* data, int size, FileType type)
: m_Data(data), m_Size(size), m_Type(type), m_Status(DataStatus::SUCCESSFUL), m_ReleaseData(false)
{}

FileData::FileData(const std::string& path, FileType type)
: m_Path(path), m_Data(nullptr), m_Size(0), m_Type(type), m_Status(DataStatus::UNLOADED), m_ReleaseData(false)
{ LoadFile(path, type); }

FileData::~FileData()
{ Clear(); }

Error FileData::LoadFile(const std::string& path, FileType type)
{
    if(type == FileType::Unknown)
        { type = s_DetectFileType(path); }

    std::string file_path = FileSystem::GetAbsolute(path);

    // https://stackoverflow.com/a/22131201
    FILE* image_file = fopen(file_path.c_str(), "r+");

    if(!image_file)
    {
        print_warning("Failed to load file '{}'", path);
        m_Status = DataStatus::FAILED;
        return ERR_FILE_LOAD;
    }

    fseek(image_file, 0, SEEK_END);
    int size = ftell(image_file);
    fclose(image_file);

    image_file = fopen(file_path.c_str(), "r+");
    unsigned char* data = new unsigned char[size];

    fread(data, sizeof(unsigned char), size, image_file);
    fclose(image_file);

    Clear();
    m_Data = data;
    m_Size = size;
    m_Type = type;
    m_Path = path;
    m_ReleaseData = true;
    m_Status = DataStatus::SUCCESSFUL;
    return OK;
}

void FileData::LoadData(const unsigned char* data, int size, FileType type)
{
    Clear();
    m_Status = DataStatus::SUCCESSFUL;
    m_Data = data;
    m_Size = size;
    m_Type = type;
}

DataStatus FileData::Status() const
{ return m_Status; }

FileType FileData::Type() const
{ return m_Type; }

const std::string& FileData::Path() const
{ return m_Path; }

bool FileData::HasPath() const
{ return !m_Path.empty(); }

void FileData::Clear()
{
    if(m_ReleaseData)
        { delete [] m_Data; }
    m_Data = nullptr;
    m_Size = 0;
    m_Path = "";
    m_Type = FileType::Unknown;
    m_Status = DataStatus::UNLOADED;
    m_ReleaseData = false;
}

std::string FileData::String() const
{ return std::string(m_Data, m_Data + m_Size); }

const unsigned char* FileData::Data() const
{ return m_Data; }

int FileData::Size() const
{ return m_Size; }

bool FileData::Empty() const
{ return (m_Data != nullptr); }
