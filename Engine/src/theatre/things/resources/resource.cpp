#include "resource.hpp"
#include "theatre/parser/thing_data.hpp"

void Resource::SetVariables(Farg<ThingData> inData)
{
    Thing::SetVariables(inData);
    std::string path = "";
    if(inData.GetVariable(path, "File", "Data", "FilePath"))
        { m_pFileData->LoadFile(path); }
    mStatus = m_pFileData->Status();
}

Shared<ThingData> Resource::GetVariables() const
{
    Shared<ThingData> data{Thing::GetVariables()};
    if(m_pFileData->HasPath())
        { data->AddVariable(m_pFileData->Path(), "File"); }
    return data;
}

Error Resource::Status() const
{ print_error_enum(mStatus); return mStatus; }

Shared<FileData> Resource::Data()
{ return m_pFileData; }
