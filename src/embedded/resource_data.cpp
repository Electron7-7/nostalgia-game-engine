#include "resource_data.hpp"
#include "images.hpp"
#include "models.hpp"
#include "fonts.hpp"
#include "filesystem/file_data.hpp"

#include <cassert>
#include <map>

struct Data_Type_Storage
{
    const unsigned char* m_Data = nullptr;
    int m_Size = 0;
    FileType m_Type = FileType::Unknown;
};

static std::map<std::string, Data_Type_Storage> s_ResourceData =
{
    { Font::Name::Audiowide,       {Font::Audiowide,       std::size(Font::Audiowide),       FileType::font_TTF}  },
    { Font::Name::Verdana,         {Font::Verdana,         std::size(Font::Verdana),         FileType::font_TTF}  },
    { Font::Name::DejaVuSansMono,  {Font::DejaVuSansMono,  std::size(Font::DejaVuSansMono),  FileType::font_TTF}  },
    { Image::Name::Missing,        {Image::Missing,        std::size(Image::Missing),        FileType::image_JPG} },
    { Image::Name::LightDebugging, {Image::LightDebugging, std::size(Image::LightDebugging), FileType::image_JPG} },
    { Image::Name::COMP04_5,       {Image::COMP04_5,       std::size(Image::COMP04_5),       FileType::image_PNG} },
    { Image::Name::LolBit,         {Image::LolBit,         std::size(Image::LolBit),         FileType::image_PNG} },
    { Model::Name::Error,          {Model::Error,          std::size(Model::Error),          FileType::model_OBJ} },
    { Model::Name::Ramiel,         {Model::Ramiel,         std::size(Model::Ramiel),         FileType::model_OBJ} },
    { Model::Name::Cube,           {Model::Cube,           std::size(Model::Cube),           FileType::model_OBJ} },
};

bool ResourceData::Exists(const std::string& name)
{ return s_ResourceData.contains(name); }

void ResourceData::AddData(const std::string& name, const FileData& data)
{
    assert(!Exists(name) && "FileData with that name already exists; please choose a different name");
    s_ResourceData[name] =
    {
        data.Data(),
        data.Size(),
        data.Type()
    };
    return;
}

bool ResourceData::GetData(FileData& output, const std::string& name)
{
    bool status = Exists(name);
    if(status)
    {
        output.LoadData
        (
            s_ResourceData.at(name).m_Data,
            s_ResourceData.at(name).m_Size,
            s_ResourceData.at(name).m_Type
        );
    }
    return status;
}
