#ifndef SPRITE_2D
#define SPRITE_2D

#include "theatre/things/thinkers/2d/visual_2d.hpp"

class Sprite2D : public Visual2D
{
public:
    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

    virtual ID TextureID() const;
    virtual void SetTextureID(ID);

protected:
    ID mTextureID{};
};

class TestAnimatedSprite2D : public Sprite2D
{
public:
    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;
    virtual void Tick() override;

protected:
    int mInterval{35};
    size_t mIndex{0};
    std::vector<ID> mTextureIDs{};
};

#endif // SPRITE_2D
