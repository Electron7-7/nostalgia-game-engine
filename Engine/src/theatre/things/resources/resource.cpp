#include "theatre/parser.hpp"

using namespace TheatreFile;

void Resource::SetVariables(Farg<ThingData> inData)
{
    Thing::SetVariables(inData);
    if(std::string path{};
        inData.get_variable(path, "File", "Data", "Path") == OK)
            { m_pFileData->LoadFile(path); }
    mStatus = m_pFileData->Status();
}

Shared<ThingData> Resource::GetVariables() const
{
    Shared<ThingData> data{Thing::GetVariables()};
    if(m_pFileData->HasPath())
        { data->set_variable(m_pFileData->Path(), "File"); }
    return data;
}

Error Resource::Status() const
{ print_error_enum(mStatus); return mStatus; }

Shared<FileData> Resource::Data()
{ return m_pFileData; }
