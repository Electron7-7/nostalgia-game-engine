#include "resource_data.hpp"
#include "printing.hpp"

#include <map>

static std::map<std::string, std::shared_ptr<const FileData>> s_ResourceData =
{
    { "VerdanaFont",          std::shared_ptr<const FileData>(&Fonts::Verdana)           },
    { "AudiowideRegularFont", std::shared_ptr<const FileData>(&Fonts::Audiowide_Regular) },
    { "DejaVuSansMonoFont",   std::shared_ptr<const FileData>(&Fonts::DejaVuSansMono)    },
    { "MissingTexture",       std::shared_ptr<const FileData>(&Images::Missing)          },
    { "DoomTexture",          std::shared_ptr<const FileData>(&Images::COMP04_5)         },
    { "ErrorModel",           std::shared_ptr<const FileData>(&Models::Error)            },
    { "RamielModel",          std::shared_ptr<const FileData>(&Models::Ramiel)           },
};

bool ResourceData::Exists(const std::string& name)
{ return s_ResourceData.contains(name); }

bool ResourceData::try_AddData(const std::string& name, std::shared_ptr<const FileData> data)
{
    if(Exists(name))
    {
        PRINT_ERROR("ResourceData::try_AddData - '{}' already exists; please choose a different name", name)
        return false;
    }
    else if(!data)
    {
        PRINT_ERROR("ResourceData::try_AddData - FileData '{}' is nullptr!", name)
        return false;
    }
    s_ResourceData[name] = data;
    return true;
}

bool ResourceData::GetData(std::shared_ptr<const FileData> output, const std::string& name)
{
    bool status = Exists(name);
    if(status)
        { output = s_ResourceData.at(name); }
    return status;
}

std::shared_ptr<const FileData> ResourceData::GetData(const std::string& name)
{
    if(!Exists(name))
        { return s_ResourceData.at(name); }
    return std::make_shared<FileData>();
}
