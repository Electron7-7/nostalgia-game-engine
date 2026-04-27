#include "./viewport_texture.hpp"
#include "things/thing_data.hpp"
#include "theatre/theatre.hpp"
#include "things/thinkers/viewport.hpp"

using namespace TheatreFile;

void ViewportTexture::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mViewportID, "Viewport");
}

Shared<ThingData> ViewportTexture::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mViewportID, "Viewport");

    return data;
}

Shared<TextureBuffer> ViewportTexture::Buffer() const
{ return Theatre::Current()->GetThinker<Viewport>(mViewportID)->GetTextureBuffer(); }

ID ViewportTexture::ViewportID() const
{ return mViewportID; }

void ViewportTexture::SetViewportID(ID inID)
{ mViewportID = inID; }

Farg<Size2D> ViewportTexture::Size() const
{ return Theatre::Current()->GetThinker<Viewport>(mViewportID)->Size(); }
