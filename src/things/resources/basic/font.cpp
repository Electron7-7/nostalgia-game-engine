#include "font.hpp"

void Font::SetupVariables(const data_t& data)
{
    Resource::SetupVariables(data);
    CreateResource();
    // g_pBackendManager->GetGraphicsBackend()->BufferFont(this);
}

SafeStatus Font::CreateResource()
{ return Status::ERROR_GENERIC; }

