#ifndef SPRITE_2D
#define SPRITE_2D

#include <Nostalgia/things/thinkers/2d/visual_2d.hpp>
#include <Nostalgia/things/resources/texture.hpp>

class Sprite2D : public Visual2D
{
public:
	SET_SUPER(Visual2D)
	SET_TYPEID(ThingType::Sprite2D)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual Shared<Texture> GetTexture() const;
    virtual void SetTexture(Shared<Texture>);

protected:
    Shared<Texture> mTexture{MakeShared<Texture>()};
    SamplerState mSampler{};
    std::string mTextureImagePath{};
};

#endif // SPRITE_2D
