#include "file_data.hpp"
#include "filesystem.hpp"

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
    std::string extension = Filesystem::GetExtension(path);
    if(!s_FileTypesByExtension.contains(extension))
        { return FileType::Unknown; }
    return s_FileTypesByExtension.at(extension);
}

SafeStatus FileData::LoadFile(const std::string& path, FileType type)
{
    if(type == FileType::Unknown)
        { type = s_DetectFileType(path); }
    if(m_ReleaseData)
        { delete [] m_Data; }

    std::string file_path = "";
    Filesystem::MakePathAbsolute(path, file_path);

    // https://stackoverflow.com/a/22131201
    FILE* image_file = fopen(file_path.c_str(), "r+");

    if(!image_file)
        { return Status::FileDataFAILED_TO_PROCESS_FILE; }

    fseek(image_file, 0, SEEK_END);
    m_Size = ftell(image_file);
    fclose(image_file);

    image_file = fopen(file_path.c_str(), "r+");
    unsigned char* data = new unsigned char[m_Size];

    fread(data, sizeof(unsigned char), m_Size, image_file);
    fclose(image_file);

    m_Data = data;
    m_Type = type;
    m_Path = path;
    m_ReleaseData = true;

    return Status::NO_ERR;
}

DataStatus FileData::Status() const
{ return m_Status; }

FileType FileData::Type() const
{ return m_Type; }

bool FileData::HasPath() const
{ return !m_Path.empty(); }

const std::string& FileData::Path() const
{ return m_Path; }

std::vector<unsigned char> FileData::Vector() const
{ return std::vector<unsigned char>(m_Data, m_Data + m_Size); }

std::string FileData::String() const
{ return std::string(m_Data, m_Data + m_Size); }

const unsigned char* FileData::data() const
{ return m_Data; }

long FileData::size() const
{ return m_Size; }

bool FileData::empty() const
{ return (m_Data != nullptr); }
