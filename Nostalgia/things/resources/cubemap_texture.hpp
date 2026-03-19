#ifndef CUBEMAP_TEXTURE_H
#define CUBEMAP_TEXTURE_H

#include <Nostalgia/things/resources/texture.hpp>

class CubemapTexture : public Texture
{
public:
    using images_t = std::array<Shared<FileData>, 6>;

    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual Error Import() override;

protected:
    images_t m_pImages{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
};

#endif // CUBEMAP_TEXTURE_H
