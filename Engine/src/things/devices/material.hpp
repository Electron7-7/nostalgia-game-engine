#ifndef MATERIAL_H
#define MATERIAL_H

#include "device.hpp"

#include <glm/glm.hpp>

class Material : public Device
{
public:
    virtual void SetVariables(const ThingData&);
    virtual ThingData GetVariables() const;

    ID GetDiffuseTexture() const;
    void SetDiffuseTexture(ID TextureID);

    ID GetSpecularTexture() const;
    void SetSpecularTexture(ID TextureID);

    glm::vec3 mColor = glm::vec3(1.0f);
    float     mAlpha = 1.0f;
    int       mSpecularSharpness = 16;
    float     mSpecularStrength = 0.0f;
    bool      mDontUseTexture = false;
    bool      mFullBright{false};

private:
    ID mDiffuseTextureID  = ID::Invalid;
    ID mSpecularTextureID = ID::Invalid;
};

#endif // MATERIAL_H
