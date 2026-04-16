#include "./sprite_3d.hpp"
#include "things/thing_data.hpp"

using namespace TheatreFile;

void Sprite3D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mTexture, "Texture", "Sprite");
}

Shared<ThingData> Sprite3D::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mTexture, "Texture");

    return data;
}

Shared<Texture> Sprite3D::GetTexture() const
{ return mTexture; }

void Sprite3D::SetTexture(Shared<Texture> inTexture)
{ mTexture = inTexture; }
