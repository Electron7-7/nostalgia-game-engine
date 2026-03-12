#ifndef TEST_ANIMATED_SPRITE_H
#define TEST_ANIMATED_SPRITE_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/theatre/things/thinkers/2d/sprite_2d.hpp>

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

#endif // TEST_ANIMATED_SPRITE_H
