#ifndef MATERIAL_H
#define MATERIAL_H

#include "../resource.hpp"

struct Material : public Resource
{
public:

    id_t GetDiffuseTexture() const;
    void SetDiffuseTexture(id_t TextureID);
    id_t GetSpecularTexture() const;
    void SetSpecularTexture(id_t TextureID);

private:
    friend class TheatreManager;
    id_t m_DiffuseTextureID  = NoID;
    id_t m_SpecularTextureID = NoID;
};

#endif // MATERIAL_H
