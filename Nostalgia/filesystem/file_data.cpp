#include "frozen/map.h"
#include "frozen/string.h"
#include <fstream>

static constexpr frozen::map<frozen::string, FileType, 8>
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
    if(!s_FileTypesByExtension.contains(frozen::string{extension}))
        { return FileType::Unknown; }
    return s_FileTypesByExtension.at(frozen::string{extension});
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

    size_t size{};
    if(!FileSystem::try_GetFileSize(file_path, size))
        { return mStatus = ERR_FILE_LOAD; }

    unsigned char* data{new unsigned char[size]};
    std::basic_ifstream<unsigned char> file_stream{file_path, std::ios_base::in | std::ios_base::binary};
    file_stream.read(data, size);
    file_stream.close();

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
