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
{ return Status::ERROR_GENERIC; }
