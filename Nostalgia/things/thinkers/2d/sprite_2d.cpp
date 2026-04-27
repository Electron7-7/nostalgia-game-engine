#include "./sprite_2d.hpp"
#include "things/thing_data.hpp"

using namespace TheatreFile;

void Sprite2D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    if(not data.get_variable(mTexture, "Texture", "Sprite"))
    {
        if(data.get_variable(mTextureImagePath, "Texture", "Sprite") == OK)
            { mTexture = ImageTexture::CreateFromImage(Image::CreateFromFile(mTextureImagePath)); }
    }
}

Shared<ThingData> Sprite2D::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mTexture, "Texture");
    data->set_variable(mTextureImagePath, "Texture");

    return data;
}

Shared<Texture> Sprite2D::GetTexture() const
{ return mTexture; }

void Sprite2D::SetTexture(Shared<Texture> inTexture)
{ mTexture = inTexture; }
