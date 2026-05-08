#ifndef TEST_ANIMATED_SPRITE_H
#define TEST_ANIMATED_SPRITE_H

#include <Nostalgia/Nostalgia.hpp>
#include <Nostalgia/things/thinkers/2d/sprite_2d.hpp>

class TestAnimatedSprite2D : public Sprite2D
{
public:
    THING_CLASS(TestAnimatedSprite2D, Sprite2D)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE
    TICK_OVERRIDE

protected:
    int mInterval{35};
    size_t mIndex{0};
    std::vector<Shared<Texture>> mTextures{};
};

#endif // TEST_ANIMATED_SPRITE_H
