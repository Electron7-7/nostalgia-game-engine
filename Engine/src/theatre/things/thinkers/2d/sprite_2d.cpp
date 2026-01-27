#include "sprite_2d.hpp"
#include "managers/manager.hpp"

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
