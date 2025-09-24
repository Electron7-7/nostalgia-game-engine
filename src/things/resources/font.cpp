#include "font.hpp"

void Font::SetupVariables(const data_t& data)
{
    Resource::SetupVariables(data);
    CreateResource();
    // g_pBackendManager->Graphics()->BufferFont(this);
}

SafeStatus Font::CreateResource()
{ return Status::ERROR_GENERIC; }

