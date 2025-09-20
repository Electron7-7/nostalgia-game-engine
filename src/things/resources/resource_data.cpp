#include "resource_data.hpp"
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
    { Fonts::Name::AudiowideRegular, {Fonts::Audiowide_Regular, std::size(Fonts::Audiowide_Regular), FileType::font_TTF}  },
    { Fonts::Name::Verdana,          {Fonts::Verdana,           std::size(Fonts::Verdana),           FileType::font_TTF}  },
    { Fonts::Name::DejaVuSansMono,   {Fonts::DejaVuSansMono,    std::size(Fonts::DejaVuSansMono),    FileType::font_TTF}  },
    { Images::Name::Missing,         {Images::Missing,          std::size(Images::Missing),          FileType::image_JPG} },
    { Images::Name::Empty,           {Images::Empty,            std::size(Images::Empty),            FileType::image_PNG} },
    { Images::Name::COMP04_5,        {Images::COMP04_5,         std::size(Images::COMP04_5),         FileType::image_PNG} },
    { Images::Name::LolBit,          {Images::LolBit,           std::size(Images::LolBit),           FileType::image_PNG} },
    { Models::Name::Error,           {Models::Error,            std::size(Models::Error),            FileType::model_OBJ} },
    { Models::Name::Ramiel,          {Models::Ramiel,           std::size(Models::Ramiel),           FileType::model_OBJ} },
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
