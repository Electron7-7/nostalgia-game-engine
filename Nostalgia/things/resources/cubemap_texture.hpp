#ifndef CUBEMAP_TEXTURE_H
#define CUBEMAP_TEXTURE_H

#include <Nostalgia/things/resources/texture.hpp>

class CubemapTexture : public Texture
{
public:
    using images_t = std::array<Shared<FileData>, 6>;

    static Shared<CubemapTexture> CreateFromMemory(std::initializer_list<FileData> inImages,
        ID inOptionalUID = {}, Sarg inOptionalName = "Untitled_Cubemap");

    SUPER(Texture)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual Error Import() override;

protected:
    images_t m_pImages{};
};

#endif // CUBEMAP_TEXTURE_H
