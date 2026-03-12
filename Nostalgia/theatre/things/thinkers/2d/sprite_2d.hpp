#ifndef SPRITE_2D
#define SPRITE_2D

#include <Nostalgia/theatre/things/thinkers/2d/visual_2d.hpp>

class Sprite2D : public Visual2D
{
public:
    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

    virtual ID TextureID() const;
    virtual void SetTextureID(ID);

protected:
    ID mTextureID{};
};

#endif // SPRITE_2D
