#ifndef VIEWPORT_TEXTURE_H
#define VIEWPORT_TEXTURE_H

#include <Nostalgia/things/resources/texture.hpp>
#include <Nostalgia/rendering/render_buffer.hpp>

class ViewportTexture : public Texture
{
public:
	SET_SUPER(Texture)
	SET_TYPEID(ThingType::ViewportTexture)
    virtual SIG_SET_VARIABLES override;
    virtual SIG_GET_VARIABLES override;

    Shared<TextureBuffer> Buffer() const override;

    virtual ID ViewportID() const;
    virtual void SetViewportID(ID);
    virtual Farg<Size2D> Size() const;

protected:
    ID mViewportID{};
};

#endif // VIEWPORT_TEXTURE_H
