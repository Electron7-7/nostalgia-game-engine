#ifndef MATERIAL_H
#define MATERIAL_H

#include "thing/resource/resource.hpp"
#include <glm/vec3.hpp>

class Material : public Resource
{
public:
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    ID DiffuseTextureID() const;
    void DiffuseTextureID(ID TextureID);

    ID SpecularTextureID() const;
    void SpecularTextureID(ID TextureID);

    glm::vec3 mColor{1.0f};
    float     mAlpha{1.0f};
    int       mSpecularSharpness{16};
    float     mSpecularStrength{0.0f};
    bool      mDontUseTexture{false};
    bool      mFullBright{false};

protected:
    ID mDiffuseTextureID{},
        mSpecularTextureID{};
};

#endif // MATERIAL_H
