#ifndef TEST_ANIMATED_SPRITE_H
#define TEST_ANIMATED_SPRITE_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/things/thinkers/2d/sprite_2d.hpp>

class TestAnimatedSprite2D : public Sprite2D
{
public:
    SUPER(Sprite2D)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual void Tick() override;

protected:
    int mInterval{35};
    size_t mIndex{0};
    std::vector<ID> mTextureIDs{};
};

#endif // TEST_ANIMATED_SPRITE_H
