#ifndef SPRITE_3D_H
#define SPRITE_3D_H

#include <Nostalgia/things/thinkers/3d/visual_3d.hpp>
#include <Nostalgia/things/resources/texture.hpp>

class Sprite3D : public Visual3D
{
public:
	SET_SUPER(Visual3D)
	SET_TYPEID(ThingType::Sprite3D)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual Shared<Texture> GetTexture() const;
    virtual void SetTexture(Shared<Texture>);

protected:
    Shared<Texture> mTexture{nullptr};
};

#endif // SPRITE_3D_H
