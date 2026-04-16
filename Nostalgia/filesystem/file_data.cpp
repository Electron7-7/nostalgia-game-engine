#include "frozen/map.h"
#include "frozen/string.h"
#include "theatre/theatre.hpp"
#include <fstream> // IWYU pragma: keep

static constinit const frozen::map<frozen::string, FileType, 9>
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

FileType FileData::sDetectFileType(Sarg inPath)
{
    if(auto found_it{s_FileTypesByExtension.find(frozen::string{FileSystem::GetExtension(inPath)})};
        found_it != s_FileTypesByExtension.end())
            { return found_it->second; }
    return FileType::Unknown;
}

FileData::FileData() = default;

FileData::FileData(const uchar* inData, size_t inSize, FileType inType, Sarg inOverridePath):
    mData{inData, inData + inSize}, mPath{inOverridePath}, mType{inType}, mStatus{OK} {}

FileData::FileData(Sarg inStrData, FileType inType, Sarg inOverridePath):
    mPath{inOverridePath}, mType{inType}, mStatus{OK}
{
    mData = Data_t(inStrData.size());
    std::copy(inStrData.begin(), inStrData.end(), mData.begin());
}

FileData::FileData(Sarg path):
    mData{}, mPath{path}, mType{FileType::Unknown}, mStatus{ERR_NOT_LOADED}
{ print_error_enum(LoadFile(path)); }

Error FileData::LoadFile(Sarg inPath)
{
    std::string file_path{inPath};
    if(not FileSystem::IsFile(inPath))
    {
        if(not FileSystem::IsFile(file_path = FileSystem::GetAbsolute(inPath)))
        {
            if(not FileSystem::IsFile(file_path = FileSystem::GetProgramDirectory() + inPath))
            {
                if(FAUTO theatre_dir{Theatre::Current()->TheatreFileDirectory()};
                    not FileSystem::IsFile(file_path = theatre_dir + "/" + inPath))
                        { return ERR_INVALID_PATH; }
            }
        }
    }

    std::ifstream file_stream{file_path, std::ios::binary};
    mData = std::vector<unsigned char>{std::istreambuf_iterator<char>(file_stream), {}};
    mType = sDetectFileType(inPath);
    mPath = file_path;
    file_stream.close();
    return mStatus = OK;
}

void FileData::LoadData(const uchar* inData, size_t inSize, FileType inType)
{
    mPath.clear();
    mData = std::vector<uchar>{inData, inData + inSize};
    mType = inType;
    mStatus = OK;
}

void FileData::clear()
{
    mPath.clear();
    mData.clear();
    mType = FileType::Unknown;
    mStatus = ERR_EMPTY;
}

FileData::Data_t& FileData::data()
{ return mData; }

Farg<FileData::Data_t> FileData::data() const
{ return mData; }

uchar* FileData::raw_data()
{ return mData.data(); }

const uchar* FileData::raw_data() const
{ return mData.data(); }

std::string FileData::raw_data_str() const
{ return std::string{mData.cbegin(), mData.cend()}; }

size_t FileData::size() const
{ return mData.size(); }

Error FileData::status() const
{ return mStatus; }

FileType FileData::file_type() const
{ return mType; }

Sarg FileData::filepath() const
{ return mPath; }

bool FileData::has_filepath() const
{ return !mPath.empty(); }

bool FileData::empty() const
{ return mData.empty(); }
