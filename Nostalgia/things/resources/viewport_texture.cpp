#include "./viewport_texture.hpp"
#include "theatre/thing_data.hpp"
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
    auto data{Texture::GetVariables()};

    data->set_variable(mViewportID, "Viewport");

    return data;
}

void ViewportTexture::Ready()
{
    Texture::Ready();
    if(not mViewportID.invalid())
    {
        auto size{Theatre::Current()->GetThinker<Viewport>(mViewportID)->Size()};
        mFormat.width  = size.w();
        mFormat.height = size.h();
    }
}

Error ViewportTexture::Import()
{
    if(mViewportID.invalid())
        { return ERR_INVALID_ID; }
    else if(not Theatre::Current()->DerivedFrom(mViewportID, ThingType::Viewport))
        { return ERR_INVALID_TYPE; }
    return OK;
}

Shared<TextureBuffer> ViewportTexture::GetBuffer() const
{ return Theatre::Current()->GetThinker<Viewport>(mViewportID)->Framebuffer()->Texture(); }

Error ViewportTexture::SetBuffer(Shared<TextureBuffer>)
{ return ERR_NOT_ALLOWED; }

ID ViewportTexture::ViewportID() const
{ return mViewportID; }

void ViewportTexture::SetViewportID(ID inID)
{ mViewportID = inID; }
