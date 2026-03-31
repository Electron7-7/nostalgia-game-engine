#ifndef VIEWPORT_TEXTURE_H
#define VIEWPORT_TEXTURE_H

#include <Nostalgia/things/resources/texture.hpp>

class ViewportTexture : public Texture
{
public:
	SET_SUPER(Texture)
	SET_TYPEID(ThingType::ViewportTexture)
    READY_OVERRIDE
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    Error Import();
    virtual Shared<TextureBuffer> GetBuffer() const final;
    virtual Error SetBuffer(Shared<TextureBuffer>) final;

    virtual ID ViewportID() const;
    virtual void SetViewportID(ID);

protected:
    ID mViewportID{};
};

#endif // VIEWPORT_TEXTURE_H
