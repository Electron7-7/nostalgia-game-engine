#ifndef MATERIAL_H
#define MATERIAL_H

#include "../resource.hpp"

struct Material : public Resource
{
public:
    rid_t GetDiffuseTexture() const;
    void SetDiffuseTexture(rid_t TextureID);
    rid_t GetSpecularTexture() const;
    void SetSpecularTexture(rid_t TextureID);

private:
    friend class TheatreManager;
    rid_t m_DiffuseTextureID  = Resource::NoRID;
    rid_t m_SpecularTextureID = Resource::NoRID;
};

#endif // MATERIAL_H
