#ifndef CUBEMAP_TEXTURE_H
#define CUBEMAP_TEXTURE_H

#include <Nostalgia/things/resources/texture.hpp>
#include <Nostalgia/things/resources/image.hpp>

class Cubemap : public Texture
{
public:
	THING_CLASS(Cubemap, Texture)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    using images_t = std::initializer_list<Shared<Image>>;

    static Shared<Cubemap> CreateFromImages(images_t inImages);

    Error UpdateLayer(Shared<Image> inImage, int inLayer);

protected:
    using ImageUIDs_t  = std::array<ID, 6>;
    using ImagePaths_t = std::array<std::string, 6>;
    Unique<ImageUIDs_t>  mInitialImageUIDs{MakeUnique<ImageUIDs_t>()};
    Unique<ImagePaths_t> mInitialImagePaths{MakeUnique<ImagePaths_t>()};
};

#endif // CUBEMAP_TEXTURE_H
