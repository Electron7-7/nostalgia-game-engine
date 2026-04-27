#ifndef SPRITE_2D
#define SPRITE_2D

#include <Nostalgia/things/thinkers/2d/visual_2d.hpp>
#include <Nostalgia/things/resources/texture.hpp>

class Sprite2D : public Visual2D
{
public:
	SET_SUPER(Visual2D)
	SET_TYPEID(ThingType::Sprite2D)
    virtual SIG_SET_VARIABLES override;
    virtual SIG_GET_VARIABLES override;

    virtual Shared<Texture> GetTexture() const;
    virtual void SetTexture(Shared<Texture>);

protected:
    Shared<Texture> mTexture{MakeShared<Texture>()};
    std::string mTextureImagePath{};
};

#endif // SPRITE_2D
