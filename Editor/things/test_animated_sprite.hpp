#ifndef TEST_ANIMATED_SPRITE_H
#define TEST_ANIMATED_SPRITE_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/things/thinkers/2d/sprite_2d.hpp>

class TestAnimatedSprite2D : public Sprite2D
{
public:
    SET_SUPER(Sprite2D)
    DEFINE_TYPEID(TestAnimatedSprite2D, __testanimatedsprite2d)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE
    TICK_OVERRIDE

protected:
    int mInterval{35};
    size_t mIndex{0};
    std::vector<ID> mTextureIDs{};
};

#endif // TEST_ANIMATED_SPRITE_H
