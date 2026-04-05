#include "./sprite_2d.hpp"
#include "things/thing_data.hpp"

using namespace TheatreFile;

void Sprite2D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mTextureID, "Texture", "Sprite");
}

Shared<ThingData> Sprite2D::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mTextureID, "Texture");

    return data;
}

ID Sprite2D::TextureID() const
{ return mTextureID; }

void Sprite2D::SetTextureID(ID inID)
{ mTextureID = inID; }
