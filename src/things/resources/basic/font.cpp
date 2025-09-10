#include "font.hpp"
#include "../data/fonts.hpp"
#include "filesystem/filesystem.hpp"

Font Font::Missing(font_TTF_Verdana);

void Font::UpdateVariables(const data_t& data)
{
    // Resource::UpdateVariables(data); // This currently doesn't do anything

    data.GetString(m_File, "File");
    data.GetEngineRef(m_Data, "Data");
}

Font::Font() = default;

Font::Font(const BinaryFileData& data)
: m_Data(data)
{}

void Font::LoadFontFile(const std::string& file)
{ m_File = file; }

void Font::LoadFontFile(const BinaryFileData& data)
{ m_Data = data; }

SafeStatus Font::try_CreateFont()
{
    if(SafeStatus::PrintCheck(Filesystem::try_ReadFileToUCharArray(m_File, m_Data)))
    {
        m_FontDataStatus = ResourceStatus::SUCCESSFUL;
        return Status::NO_ERR;
    }
    m_FontDataStatus = ResourceStatus::FAILED;
    return Status::FontFONT_FILE_FAILED_TO_LOAD;
}

const BinaryFileData& Font::GetData() const
{
    if(m_FontDataStatus != ResourceStatus::SUCCESSFUL)
        { return Font::Missing.m_Data; }
    return m_Data;
}
