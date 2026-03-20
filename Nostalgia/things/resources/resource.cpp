#include "theatre/theatre.hpp"

using namespace TheatreFile;

void Resource::SetVariables(Farg<ThingData> inData)
{
    Thing::SetVariables(inData);
    if(std::string path{}; inData.get_variable(path, "File", "Data", "Path") == OK)
        { mStatus = try_LoadFileDataFromVariable(path, m_pFileData); }
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

Error Resource::try_LoadFileDataFromVariable(Sarg inPath, Shared<FileData>& outData)
{
    std::string file_path{inPath};
    if(Theatre::Current()->WasLoadedFromFile() and FileSystem::IsFile(Theatre::Current()->TheatreFileDirectory() + inPath))
        { file_path = Theatre::Current()->TheatreFileDirectory() + inPath; }
    else if(FileSystem::IsFile(FileSystem::GetProgramDirectory() + inPath))
        { file_path = FileSystem::GetProgramDirectory() + inPath; }
    else if(FileSystem::IsFile(FileSystem::GetAbsolute(inPath)))
        { file_path = FileSystem::GetAbsolute(inPath); }
    else if(!FileSystem::IsFile(inPath))
        { return ERR_INVALID_PATH; }
    return outData->LoadFile(file_path);
}
