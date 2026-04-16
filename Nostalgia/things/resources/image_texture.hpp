#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include <Nostalgia/things/resources/texture.hpp>
#include <Nostalgia/things/resources/image.hpp>

class ImageTexture : public Texture
{
public:
    static Shared<ImageTexture> CreateFromImage(Farg<Shared<Image>>);

    SET_SUPER(Texture)
    SET_TYPEID(ThingType::ImageTexture)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    void SetImage(Shared<Image>);
    void UpdateImage(Shared<Image>);

protected:
    ID mInitialImageID{};
    std::string mInitialImagePath{};
};

#endif // IMAGE_TEXTURE_H
