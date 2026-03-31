#ifndef CUBEMAP_TEXTURE_H
#define CUBEMAP_TEXTURE_H

#include <Nostalgia/things/resources/texture.hpp>
#include <Nostalgia/things/resources/image.hpp>

class CubemapTexture : public Texture
{
public:
    using images_t = std::initializer_list<Shared<Image>>;

    static Shared<CubemapTexture> CreateFromImages(images_t inImages);

	SET_SUPER(Texture)
	SET_TYPEID(ThingType::CubemapTexture)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    Error UpdateLayer(Shared<Image> inImage, int inLayer);

protected:
    bool mInitialized{false};
    ID mInitialImageIDs[6]{ID::Invalid,
        ID::Invalid,
        ID::Invalid,
        ID::Invalid,
        ID::Invalid,
        ID::Invalid
    };
};

#endif // CUBEMAP_TEXTURE_H
