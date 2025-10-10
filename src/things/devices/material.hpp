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
    glm::vec4 mDebugHighlight = glm::vec4(0.9f, 0.2f, 1.0f, 0.0f);

private:
    ID mDiffuseTextureID  = ID::None;
    ID mSpecularTextureID = ID::None;
};

#endif // MATERIAL_H
