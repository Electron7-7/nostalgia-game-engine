#include "./sprite_2d.hpp"
#include "things/thing_data.hpp"

using namespace TheatreFile;

void Sprite2D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mTexture, "Texture", "Sprite");
}

Shared<ThingData> Sprite2D::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mTexture, "Texture");

    return data;
}

Shared<Texture> Sprite2D::GetTexture() const
{ return mTexture; }

void Sprite2D::SetTexture(Shared<Texture> inTexture)
{ mTexture = inTexture; }
