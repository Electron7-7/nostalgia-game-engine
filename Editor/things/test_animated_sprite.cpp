#include "./test_animated_sprite.hpp"
#include <Nostalgia/things/thing_data.hpp>
#include <Nostalgia/managers/manager.hpp>

using namespace TheatreFile;

void TestAnimatedSprite2D::SetVariables(Farg<ThingData> data)
{
    Sprite2D::SetVariables(data);

    data.get_variable(mInterval, "Interval");

    int i{0};
    ID id{};
    while(data.get_variable(id, std::format("Texture{}", i)) == OK)
    {
        mTextureIDs.push_back(id);
        ++i;
    }
    if(!mTextureIDs.empty())
        { mTextureID = mTextureIDs.front(); }
}

Shared<ThingData> TestAnimatedSprite2D::GetVariables() const
{
    auto data{Sprite2D::GetVariables()};

    data->set_variable(mInterval, "Interval");

    int i{0};
    for(ID id : mTextureIDs)
        { data->set_variable(id, std::format("Texture{}", i)); }

    return data;
}

void TestAnimatedSprite2D::Tick()
{
    if(!(Manager::TickNumber() % mInterval))
    {
        mIndex = (++mIndex >= mTextureIDs.size())
            ? 0
            : mIndex;
        mTextureID = mTextureIDs[mIndex];
    }
}
