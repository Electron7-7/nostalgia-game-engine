#ifndef SPRITE_3D_H
#define SPRITE_3D_H

#include <Nostalgia/things/thinkers/3d/visual_3d.hpp>
#include <Nostalgia/things/resources/texture.hpp>
#include <Nostalgia/things/thing_factory.hpp>

class Sprite3D : public Visual3D
{
public:
	THING_CLASS(Sprite3D, Visual3D)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual Shared<Texture> GetTexture() const;
    virtual void SetTexture(Shared<Texture>);

protected:
    Shared<Texture> mTexture{ThingFactory::Invalid<Texture>()};
    SamplerState mSampler{};
    std::string mTextureImagePath{};
};

#endif // SPRITE_3D_H
