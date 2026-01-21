#include "file_data.hpp"
#include "filesystem.hpp"
#include "core/uid.hpp"
#include "core/printing.hpp"
#include "embedded/models.hpp"
#include "embedded/images.hpp"
#include "thirdparty/frozen/map.h"
#include <map>

static std::map<ID, Shared<FileData>>
sReservedFileData{
    {UID::m_Error,            MakeShared<FileData>(Models::Error,      std::size(Models::Error),      FileType::model_OBJ)},
    {UID::m_Cube,             MakeShared<FileData>(Models::Cube,       std::size(Models::Cube),       FileType::model_OBJ)},
    {UID::m_Ramiel,           MakeShared<FileData>(Models::Ramiel,     std::size(Models::Ramiel),     FileType::model_OBJ)},
    {UID::m_Camera3D,         MakeShared<FileData>(Models::Camera,     std::size(Models::Camera),     FileType::model_OBJ)},
    {UID::m_DebugAxis,        MakeShared<FileData>(Models::DebugAxis,  std::size(Models::DebugAxis),  FileType::model_OBJ)},
    {UID::t_Missing,          MakeShared<FileData>(Images::Missing,    std::size(Images::Missing),    FileType::image_PNG)},
    {UID::t_LolBit,           MakeShared<FileData>(Images::LolBit,     std::size(Images::LolBit),     FileType::image_PNG)},
    {UID::t_LightDebug,       MakeShared<FileData>(Images::LightDebug, std::size(Images::LightDebug), FileType::image_JPG)},
    {UID::t_COMP04_5,         MakeShared<FileData>(Images::COMP04_5,   std::size(Images::COMP04_5),   FileType::image_PNG)},
    {UID::t_ShittySkybox,     MakeShared<FileData>(Images::SkyboxXn,   std::size(Images::SkyboxXn),   FileType::image_PNG)},
    {UID::t_ShittySkybox[]+1, MakeShared<FileData>(Images::SkyboxXp,   std::size(Images::SkyboxXp),   FileType::image_PNG)},
    {UID::t_ShittySkybox[]+2, MakeShared<FileData>(Images::SkyboxYp,   std::size(Images::SkyboxYp),   FileType::image_PNG)},
    {UID::t_ShittySkybox[]+3, MakeShared<FileData>(Images::SkyboxYn,   std::size(Images::SkyboxYn),   FileType::image_PNG)},
    {UID::t_ShittySkybox[]+4, MakeShared<FileData>(Images::SkyboxZn,   std::size(Images::SkyboxZn),   FileType::image_PNG)},
    {UID::t_ShittySkybox[]+5, MakeShared<FileData>(Images::SkyboxZp,   std::size(Images::SkyboxZp),   FileType::image_PNG)},
};

static constexpr frozen::map<std::string, FileType, 8>
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
    mData{data},
    mSize{size},
    mType{type},
    mStatus{OK}{}

FileData::FileData(Farg<std::string> path, FileType type):
    mData{nullptr},
    mSize{0},
    mPath{path},
    mType{type},
    mStatus{ERR_NOT_LOADED}
{ LoadFile(path, type); }

FileData::~FileData()
{ clear(); }

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
        return mStatus = ERR_FILE_LOAD;
    }

    fseek(image_file, 0, SEEK_END);
    long size{ftell(image_file)};
    fclose(image_file);

    image_file = fopen(file_path.c_str(), "r+");
    unsigned char* data{new unsigned char[size]};

    fread(data, sizeof(unsigned char), size, image_file);
    fclose(image_file);

    clear();
    mData = data;
    mSize = size;
    mType = type;
    mPath = path;
    mReleaseData = true;
    return mStatus = OK;
}

void FileData::LoadData(const unsigned char* data, int size, FileType type)
{
    clear();
    mStatus = OK;
    mData = data;
    mSize = size;
    mType = type;
}

Error FileData::Status() const
{ return mStatus; }

FileType FileData::Type() const
{ return mType; }

Farg<std::string> FileData::Path() const
{ return mPath; }

bool FileData::HasPath() const
{ return !mPath.empty(); }

void FileData::clear()
{
    if(mReleaseData)
        { delete [] mData; }
    mReleaseData = false;
    mData = nullptr;
    mSize = 0;
    mPath = "";
    mType = FileType::Unknown;
    mStatus = ERR_NOT_LOADED;
}

std::string FileData::DataString() const
{ return std::string(mData, mData + mSize); }

const unsigned char* FileData::Data() const
{ return mData; }

int FileData::Size() const
{ return mSize; }

bool FileData::empty() const
{ return (mData != nullptr); }
