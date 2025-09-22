#include "texture.hpp"
#include "../resource_data.hpp"
#include "filesystem/file_data.hpp"
#include "managers/backend_manager.hpp"

Texture Texture::Missing({Images::Missing, std::size(Images::Missing), FileType::image_JPG});
Texture Texture::Empty({Images::Empty, std::size(Images::Empty), FileType::image_PNG});

void Texture::SetupVariables(const data_t& data)
{
    Resource::SetupVariables(data);
    CreateResource();
    g_pBackendManager->GetGraphicsBackend()->BufferTexture(this);
}

SafeStatus Texture::CreateResource()
{
    if(m_FileData.Status() != DataStatus::SUCCESSFUL)
    {
        // ResourceData::GetData(m_FileData, Images::Name::Missing);
        m_Status = ResourceStatus::FAILED;
        return Status::ResourceBAD_FILE_DATA;
    }
    m_Status = ResourceStatus::SUCCESSFUL;
    return Status::NO_ERR;
}
