#ifndef SPRITE_2D
#define SPRITE_2D

#include <Nostalgia/things/thinkers/2d/visual_2d.hpp>

class Sprite2D : public Visual2D
{
public:
    SUPER(Visual2D)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual ID TextureID() const;
    virtual void SetTextureID(ID);

protected:
    ID mTextureID{};
};

#endif // SPRITE_2D
