#include "frozen/map.h"
#include "frozen/string.h"
#include "managers/theatre_manager.hpp"
#include <fstream> // IWYU pragma: keep

static constexpr frozen::map<frozen::string, FileType, 9>
s_FileTypesByExtension{
    { ".otf",  FileType::font_OTF   },
    { ".ttf",  FileType::font_TTF   },
    { ".png",  FileType::image_PNG  },
    { ".jpg",  FileType::image_JPG  },
    { ".jpeg", FileType::image_JPG  },
    { ".nt",   FileType::theatre_NT },
    { ".obj",  FileType::model_OBJ  },
    { ".frag", FileType::glsl_FRAG  },
    { ".vert", FileType::glsl_VERT  },
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
    mData{data, data + size},
    // mSize{size},
    mType{type},
    mStatus{OK}{}

FileData::FileData(Farg<std::string> path, FileType type):
    mData{},
    // mSize{0},
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

    std::string file_path{path};
    if(not FileSystem::IsFile(path))
    {
        if(not FileSystem::IsFile(file_path = FileSystem::GetAbsolute(path)))
        {
            if(not FileSystem::IsFile(file_path = FileSystem::GetProgramDirectory() + path))
            {
                if(auto* theatre{g_pTheatreManager->Current()}; theatre and theatre->WasLoadedFromFile()
                    and not FileSystem::IsFile(file_path = theatre->TheatreFileDirectory() + path))
                        { return ERR_INVALID_PATH; }
            }
        }
    }

    std::ifstream file_stream{file_path, std::ios::binary};
    mData = std::vector<unsigned char>{std::istreambuf_iterator<char>(file_stream), {}};
    mType = type;
    mPath = path;
    file_stream.close();
    return mStatus = OK;
}

void FileData::LoadData(const unsigned char* data, int size, FileType type)
{
    clear();
    mStatus = OK;
    mData = std::vector<uchar>{data, data + size};
    // mSize = size;
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
    mData.clear();
    mPath = "";
    mType = FileType::Unknown;
    mStatus = ERR_NOT_LOADED;
}

std::string FileData::DataString() const
{ return std::string(mData.begin(), mData.end()); }

const unsigned char* FileData::Data() const
{ return mData.data(); }

int FileData::Size() const
{ return mData.size(); }

bool FileData::empty() const
{ return mData.empty(); }
