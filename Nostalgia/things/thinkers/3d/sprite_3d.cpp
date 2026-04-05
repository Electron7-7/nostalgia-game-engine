#include "./sprite_3d.hpp"
#include "things/thing_data.hpp"

using namespace TheatreFile;

void Sprite3D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mTextureID, "Texture", "Sprite");
}

Shared<ThingData> Sprite3D::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mTextureID, "Texture");

    return data;
}

ID Sprite3D::TextureID() const
{ return mTextureID; }

void Sprite3D::SetTextureID(ID inID)
{ mTextureID = inID; }
