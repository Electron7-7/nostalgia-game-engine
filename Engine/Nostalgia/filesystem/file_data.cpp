#ifdef NOSTALGIA_GCC_PCH
#   include <Nostalgia/Nostalgia.hpp>
#endif

#include "frozen/map.h"

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
