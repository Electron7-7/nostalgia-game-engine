#ifndef VIEWPORT_TEXTURE_H
#define VIEWPORT_TEXTURE_H

#include <Nostalgia/theatre/things/resources/texture.hpp>

class ViewportTexture : public Texture
{
public:
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE
    READY_OVERRIDE

    virtual Error Import() override;
    virtual Shared<TextureBuffer> GetBuffer() const override;
    virtual Error SetBuffer(Shared<TextureBuffer>) override;

    virtual ID ViewportID() const;
    virtual void SetViewportID(ID);

protected:
    ID mViewportID{};
};

#endif // VIEWPORT_TEXTURE_H
