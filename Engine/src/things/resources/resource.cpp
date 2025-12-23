#include "resource.hpp"
#include "theatre/parser/thing_data.hpp"
#include "filesystem/file_data.hpp"

Resource::Resource():
    m_pFileData{MakeShared<FileData>()} {}

Resource::Resource(Shared<FileData> Data):
    m_pFileData{Data} {}

void Resource::SetVariables(Farg<ThingData> inData)
{
    Thing::SetVariables(inData);
    std::string path = "";
    if(inData.GetVariable(path, "File", "Data", "FilePath"))
        { m_pFileData->LoadFile(path); }
}

Shared<ThingData> Resource::GetVariables() const
{
    Shared<ThingData> data{Thing::GetVariables()};
    data->AddVariable(m_pFileData->Path(), "File");
    return data;
}

Shared<FileData> Resource::Data()
{ return m_pFileData; }
