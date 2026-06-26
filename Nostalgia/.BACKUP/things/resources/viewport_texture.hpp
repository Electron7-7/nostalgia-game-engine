#ifndef VIEWPORT_TEXTURE_H
#define VIEWPORT_TEXTURE_H

#include <Nostalgia/things/thinkers/viewport.hpp>
#include <Nostalgia/things/resources/texture.hpp>

class ViewportTexture : public Texture
{
public:
	THING_CLASS(ViewportTexture, Texture)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    Shared<TextureBuffer> Buffer() const override;

    virtual ID ViewportID() const;
    virtual void SetViewportID(ID);
    virtual Farg<Size2D<int>> Size() const;

protected:
    Shared<Viewport> m_pViewport{Viewport::Invalid()};
};

#endif // VIEWPORT_TEXTURE_H
