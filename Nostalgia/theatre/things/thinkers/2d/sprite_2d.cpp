#include "sprite_2d.hpp"
#include "managers/manager.hpp"

using namespace TheatreFile;

void Sprite2D::SetVariables(Farg<ThingData> data)
{
    Visual2D::SetVariables(data);

    data.get_variable(mTextureID, "Texture", "Sprite");
}

Shared<ThingData> Sprite2D::GetVariables() const
{
    auto data{Visual2D::GetVariables()};

    data->set_variable(mTextureID, "Texture");

    return data;
}

ID Sprite2D::TextureID() const
{ return mTextureID; }

void Sprite2D::SetTextureID(ID inID)
{ mTextureID = inID; }
