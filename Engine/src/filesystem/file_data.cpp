#include "file_data.hpp"
#include "core/uid.hpp"
#include "embedded/models.hpp"
#include "embedded/images.hpp"
#include "filesystem.hpp"
#include "core/printing.hpp"
#include "core/error.hpp"
#include <map>

static std::map<ID, Shared<FileData>>
sReservedFileData{
    {UID::m_Error,      MakeShared<FileData>(Models::Error,      std::size(Models::Error),      FileType::model_OBJ)},
    {UID::m_Cube,       MakeShared<FileData>(Models::Cube,       std::size(Models::Cube),       FileType::model_OBJ)},
    {UID::m_Ramiel,     MakeShared<FileData>(Models::Ramiel,     std::size(Models::Ramiel),     FileType::model_OBJ)},
    {UID::m_Camera3D,   MakeShared<FileData>(Models::Camera,     std::size(Models::Camera),     FileType::model_OBJ)},
    {UID::i_Missing,    MakeShared<FileData>(Images::Missing,    std::size(Images::Missing),    FileType::image_PNG)},
    {UID::i_LolBit,     MakeShared<FileData>(Images::LolBit,     std::size(Images::LolBit),     FileType::image_PNG)},
    {UID::i_LightDebug, MakeShared<FileData>(Images::LightDebug, std::size(Images::LightDebug), FileType::image_JPG)},
    {UID::i_COMP04_5,   MakeShared<FileData>(Images::COMP04_5,   std::size(Images::COMP04_5),   FileType::image_PNG)},
};

static std::map<std::string, FileType>
s_FileTypesByExtension{
    { ".otf",  FileType::font_OTF  },
    { ".ttf",  FileType::font_TTF  },
    { ".png",  FileType::image_PNG },
    { ".jpg",  FileType::image_JPG },
    { ".jpeg", FileType::image_JPG },
    { ".obj",  FileType::model_OBJ },
    { ".frag", FileType::glsl_FRAG },
    { ".vert", FileType::glsl_VERT },
};

FileType FileData::s_DetectFileType(Farg<std::string> path)
{
    std::string extension = FileSystem::GetExtension(path);
    if(!s_FileTypesByExtension.contains(extension))
        { return FileType::Unknown; }
    return s_FileTypesByExtension.at(extension);
}

Shared<FileData> FileData::s_GetReservedFileData(uint inReservedUID)
{
    if(auto type{UID::GetReservedType(inReservedUID)};
        type != UID::ReservedType::NotReserved)
        { return sReservedFileData.at(inReservedUID); }
    print_warning("Invalid UID");
    return MakeShared<FileData>();
}

FileData::FileData() = default;

FileData::FileData(const unsigned char* data, int size, FileType type):
    m_Data{data},
    m_Size{size},
    m_Type{type},
    m_Status{DataStatus::SUCCESSFUL}{}

FileData::FileData(Farg<std::string> path, FileType type):
    m_Path{path},
    m_Data{nullptr},
    m_Size{0},
    m_Type{type},
    m_Status{DataStatus::UNLOADED}
{ LoadFile(path, type); }

FileData::~FileData()
{ Clear(); }

Error FileData::LoadFile(Farg<std::string> path, FileType type)
{
    if(type == FileType::Unknown)
        { type = s_DetectFileType(path); }

    std::string file_path{FileSystem::GetAbsolute(path)};

    // https://stackoverflow.com/a/22131201
    FILE* image_file{fopen(file_path.c_str(), "r+")};

    if(!image_file)
    {
        print_error("Failed to load file '{}'", path);
        m_Status = DataStatus::FAILED;
        return ERR_FILE_LOAD;
    }

    fseek(image_file, 0, SEEK_END);
    long size{ftell(image_file)};
    fclose(image_file);

    image_file = fopen(file_path.c_str(), "r+");
    unsigned char* data{new unsigned char[size]};

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

Farg<std::string> FileData::Path() const
{ return m_Path; }

bool FileData::HasPath() const
{ return !m_Path.empty(); }

void FileData::Clear()
{
    if(m_ReleaseData)
        { delete [] m_Data; }
    m_ReleaseData = false;
    m_Data = nullptr;
    m_Size = 0;
    m_Path = "";
    m_Type = FileType::Unknown;
    m_Status = DataStatus::UNLOADED;
}

std::string FileData::String() const
{ return std::string(m_Data, m_Data + m_Size); }

const unsigned char* FileData::Data() const
{ return m_Data; }

int FileData::Size() const
{ return m_Size; }

bool FileData::Empty() const
{ return (m_Data != nullptr); }
