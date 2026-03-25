using namespace TheatreFile;

void Resource::SetVariables(Farg<ThingData> inData)
{
    Super::SetVariables(inData);
    if(std::string path{}; inData.get_variable(path, "File", "Data", "Path") == OK)
        { mStatus = m_pFileData->LoadFile(path); }
}

Shared<ThingData> Resource::GetVariables() const
{
    Shared<ThingData> data{Super::GetVariables()};
    if(m_pFileData->HasPath())
        { data->set_variable(m_pFileData->Path(), "File"); }
    return data;
}

Error Resource::Status() const
{ print_error_enum(mStatus); return mStatus; }

Shared<FileData> Resource::Data()
{ return m_pFileData; }
