#include "./sprite_3d.hpp"

using namespace TheatreFile;

void Sprite3D::SetVariables(Farg<ThingData> data)
{
    Visual3D::SetVariables(data);

    data.get_variable(mTextureID, "Texture", "Sprite");
}

Shared<ThingData> Sprite3D::GetVariables() const
{
    auto data{Visual3D::GetVariables()};

    data->set_variable(mTextureID, "Texture");

    return data;
}

ID Sprite3D::TextureID() const
{ return mTextureID; }

void Sprite3D::SetTextureID(ID inID)
{ mTextureID = inID; }
