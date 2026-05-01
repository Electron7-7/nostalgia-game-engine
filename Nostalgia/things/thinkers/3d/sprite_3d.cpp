#include "./sprite_3d.hpp"
#include "things/thing_data.hpp"
#include "things/resources/image_texture.hpp"

using namespace TheatreFile;

void Sprite3D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    if(not data.get_variable(mTexture, "Texture", "Sprite"))
    {
        if(data.get_variable(mTextureImagePath, "Texture", "Sprite") == OK)
            { mTexture = ImageTexture::CreateFromImage(Image::CreateFromFile({mTextureImagePath})); }
    }

    mSampler.SetVariables(data);
    mTexture->Buffer()->SetSamplerState(mSampler);
}

Shared<ThingData> Sprite3D::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mTexture, "Texture");
    if(not mTextureImagePath.empty())
        { data->set_variable(mTextureImagePath, "Texture"); }
    mSampler.GetVariables(data);

    return data;
}

Shared<Texture> Sprite3D::GetTexture() const
{ return mTexture; }

void Sprite3D::SetTexture(Shared<Texture> inTexture)
{ mTexture = inTexture; }
