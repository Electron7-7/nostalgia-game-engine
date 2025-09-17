#include "texture.hpp"
#include "../resource_data.hpp"
#include "filesystem/file_data.hpp"
#include "managers/backend_manager.hpp"

Texture Texture::Missing(&Images::Missing);

void Texture::SetupVariables(const data_t& data)
{
    Resource::SetupVariables(data);
    CreateResource();
    g_pBackendManager->GetGraphicsBackend()->BufferTexture(this);
}

SafeStatus Texture::CreateResource()
{
    if(Data()->Status() != DataStatus::SUCCESSFUL)
    {
        SetData(ResourceData::GetData("MissingTexture"));
        return Status::ResourceBAD_FILE_DATA;
    }
    return Status::NO_ERR;
}
