#include "./test_animated_sprite.hpp"
#include <Nostalgia/things/thing_data.hpp>
#include <Nostalgia/managers/manager.hpp>

using namespace TheatreFile;

void TestAnimatedSprite2D::SetVariables(Farg<ThingData> data)
{
    Sprite2D::SetVariables(data);

    data.get_variable(mInterval, "Interval");

    int i{0};
    Shared<Texture> _tex{nullptr};
    while(data.get_variable(_tex, std::format("Texture{}", i)) == OK)
    {
        if(i >= mTextures.size())
            { mTextures.push_back(_tex); }
        else
            { mTextures[i] = _tex; }
        ++i;
    }
    if(not mTextures.empty())
        { mTexture = mTextures.front(); }
    mTexture->Buffer()->SetSamplerState(mSampler);
}

Shared<ThingData> TestAnimatedSprite2D::GetVariables() const
{
    auto data{Sprite2D::GetVariables()};

    data->hide_from_editor("Texture");
    data->set_variable(mInterval, "Interval");

    int i{0};
    for(FAUTO texture : mTextures)
        { data->set_variable(texture, std::format("Texture{}", i++)); }

    return data;
}

void TestAnimatedSprite2D::Tick()
{
    if(!(Manager::TickNumber() % mInterval))
    {
        mIndex = (++mIndex >= mTextures.size())
            ? 0
            : mIndex;
        mTexture = mTextures[mIndex];
        mTexture->Buffer()->SetSamplerState(mSampler);
    }
}
