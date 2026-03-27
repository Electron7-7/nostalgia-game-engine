#ifndef SPRITE_3D_H
#define SPRITE_3D_H

#include <Nostalgia/things/thinkers/3d/visual_3d.hpp>

class Sprite3D : public Visual3D
{
public:
	SET_SUPER(Visual3D)
	SET_TYPEID(ThingType::Sprite3D)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual ID TextureID() const;
    virtual void SetTextureID(ID);

protected:
    ID mTextureID{};
};

#endif // SPRITE_3D_H
