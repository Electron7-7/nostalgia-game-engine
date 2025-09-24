#include "texture.hpp"
#include "filesystem/file_data.hpp"
#include "managers/backend_manager.hpp"

void Texture::SetupVariables(const data_t& data)
{
    Resource::SetupVariables(data);
    CreateResource();
    g_pBackendManager->Graphics()->BufferTexture(this);
}

SafeStatus Texture::CreateResource()
{
    if(m_FileData.Status() != DataStatus::SUCCESSFUL)
    {
        m_Status = ResourceStatus::FAILED;
        return Status::ResourceBAD_FILE_DATA;
    }
    m_Status = ResourceStatus::SUCCESSFUL;
    return Status::NO_ERR;
}
