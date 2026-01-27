#include "sprite_2d.hpp"

using namespace TheatreFile;

void Sprite2D::SetVariables(Farg<ThingData> data)
{
    Visual2D::SetVariables(data);

    data.get_variable(mTextureID, "Texture", "Sprite");
    data.get_variable(mWireframe, "Wireframe", "WireFrame");
}

Shared<ThingData> Sprite2D::GetVariables() const
{
    auto data{Visual2D::GetVariables()};

    data->set_variable(mTextureID, "Texture");
    data->set_variable(mWireframe, "Wireframe");

    return data;
}

ID Sprite2D::TextureID() const
{ return mTextureID; }

void Sprite2D::SetTextureID(ID inID)
{ mTextureID = inID; }

bool Sprite2D::Wireframe() const
{ return mWireframe; }

void Sprite2D::SetWireframe(bool inWireframe)
{ mWireframe = inWireframe; }
