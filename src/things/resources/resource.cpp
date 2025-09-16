#include "resource.hpp"
#include "theatre/data_t.hpp"

Resource::Resource() = default;

Resource::Resource(const FileData* data)
: m_FileData(std::shared_ptr<const FileData>(data))
{
    CreateResource();
}

void Resource::SetupVariables(const data_t& data)
{
    std::string path = "";
    if(data.GetString(path, "File"))
        { m_FileData = std::make_shared<FileData>(path); }
    data.GetFileData(m_FileData, "Data");
}
