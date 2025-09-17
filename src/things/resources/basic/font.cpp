#include "font.hpp"
#include "../resource_data.hpp"

Font Font::Default({Fonts::Verdana, std::size(Fonts::Verdana), FileType::font_TTF});

void Font::SetupVariables(const data_t& data)
{
    Resource::SetupVariables(data);
    CreateResource();
    // g_pBackendManager->GetGraphicsBackend()->BufferFont(this);
}

SafeStatus Font::CreateResource()
{ return Status::ERROR_GENERIC; }

