#include "resource.hpp"
#include "filesystem/file_data.hpp"
#include "theatre/data_t.hpp"

Resource::Resource()
: m_FileData()
{}

Resource::Resource(const FileData& data)
: m_FileData(data)
{}

void Resource::SetupVariables(const data_t& data)
{
    std::string path = "";
    if(data.GetString(path, "File"))
        { m_FileData.LoadFile(path); }
    data.GetFileData(m_FileData, "Data");
}
