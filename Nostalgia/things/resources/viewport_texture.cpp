#include "./viewport_texture.hpp"
#include "things/thing_data.hpp"
#include "things/thing_factory.hpp"

using namespace TheatreFile;

void ViewportTexture::InitVariables()
{
    Super::InitVariables();
}

void ViewportTexture::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(m_pViewport, "Viewport");
}

Shared<ThingData> ViewportTexture::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(m_pViewport, "Viewport");

    return data;
}

Shared<TextureBuffer> ViewportTexture::Buffer() const
{ return m_pViewport->GetTextureBuffer(); }

ID ViewportTexture::ViewportID() const
{ return m_pViewport->uid(); }

void ViewportTexture::SetViewportID(ID inID)
{ m_pViewport = ThingFactory::GetThing<Viewport>(inID); }

Farg<Size2D> ViewportTexture::Size() const
{ return m_pViewport->Size(); }
