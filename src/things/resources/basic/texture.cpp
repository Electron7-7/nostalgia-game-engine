#include "texture.hpp"
#include "../data/images.hpp"
#include "filesystem/filesystem.hpp"

const Texture Texture::Missing(image_JPG_MISSINGTEXTURE);

void Texture::UpdateVariables(const data_t& data)
{
    // Resource::UpdateVariables(data); // This currently doesn't do anything

    data.GetString(m_File, "File");
    data.GetEngineRef(m_Data, "Data");
}

Texture::Texture() = default;

Texture::Texture(const BinaryFileData& texture_data)
: m_Data(texture_data)
{}

void Texture::LoadTextureFile(const std::string& file)
{ m_File = file; }

void Texture::LoadTextureData(const BinaryFileData& data)
{ m_Data = data; }

const BinaryFileData& Texture::GetData() const
{
    if(m_TextureDataStatus != ResourceStatus::SUCCESSFUL)
        { return Texture::Missing.m_Data; }
    return m_Data;
}

SafeStatus Texture::try_CreateTexture()
{
    if(SafeStatus::PrintCheck(Filesystem::try_ReadFileToUCharArray(m_File, m_Data)))
    {
        m_TextureDataStatus = ResourceStatus::SUCCESSFUL;
        return Status::NO_ERR;
    }
    m_TextureDataStatus = ResourceStatus::FAILED;
    return Status::TextureIMAGE_FILE_FAILED_TO_LOAD;
}
