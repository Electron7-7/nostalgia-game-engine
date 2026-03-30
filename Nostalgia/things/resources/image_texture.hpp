#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include <Nostalgia/things/resources/texture.hpp>
#include <Nostalgia/things/resources/image.hpp>

class ImageTexture : public Texture
{
public:
    SET_SUPER(Texture)
    SET_TYPEID(ThingType::ImageTexture)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE
    SHUTDOWN_OVERRIDE

    virtual Error Import() override;

protected:
    Shared<Image> m_pImageResource{nullptr};
};

#endif // IMAGE_TEXTURE_H
