#include "./viewport_texture.hpp"
#include "theatre/theatre.hpp"
#include "theatre/things/thinkers/viewport.hpp"

using namespace TheatreFile;

void ViewportTexture::SetVariables(Farg<ThingData> data)
{
    Thing::SetVariables(data);

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
        auto size{my_theatre()->GetThinker<Viewport>(mViewportID)->Size()};
        mFormat.width  = size.w();
        mFormat.height = size.h();
    }
}

Error ViewportTexture::Import()
{
    if(mViewportID.invalid())
        { return ERR_INVALID_ID; }
    else if(not my_theatre()->DerivedFrom(mViewportID, ThingType::Viewport))
        { return ERR_INVALID_TYPE; }
    return OK;
}

Shared<TextureBuffer> ViewportTexture::GetBuffer() const
{ return my_theatre()->GetThinker<Viewport>(mViewportID)->Framebuffer()->Texture(); }

Error ViewportTexture::SetBuffer(Shared<TextureBuffer>)
{ return ERR_NOT_ALLOWED; }

ID ViewportTexture::ViewportID() const
{ return mViewportID; }

void ViewportTexture::SetViewportID(ID inID)
{ mViewportID = inID; }
