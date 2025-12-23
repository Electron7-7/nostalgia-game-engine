#ifdef FWD_DCL
    class Material;
#elif !defined MATERIAL_H
#define MATERIAL_H

#include "device.hpp"

#include <glm/vec3.hpp>

class Material : public Device
{
public:
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    ID GetDiffuseTexture() const;
    void SetDiffuseTexture(ID TextureID);

    ID GetSpecularTexture() const;
    void SetSpecularTexture(ID TextureID);

    glm::vec3 mColor{1.0f};
    float     mAlpha{1.0f};
    int       mSpecularSharpness{16};
    float     mSpecularStrength{0.0f};
    bool      mDontUseTexture{false};
    bool      mFullBright{false};

protected:
    ID mDiffuseTextureID{};
    ID mSpecularTextureID{};
};

#endif // MATERIAL_H
