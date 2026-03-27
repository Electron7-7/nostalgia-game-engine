#ifndef MATERIAL_H
#define MATERIAL_H

#include <Nostalgia/things/resources/resource.hpp>

class Material : public Resource
{
public:
	SET_SUPER(Resource)
	SET_TYPEID(ThingType::Material)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    ID DiffuseTextureID() const;
    void DiffuseTextureID(ID TextureID);

    ID SpecularTextureID() const;
    void SpecularTextureID(ID TextureID);

    float SpecularStrength() const;
    void SpecularStrength(float);

    glm::vec3 mColor{1.0f};
    float     mAlpha{1.0f};
    int       mSpecularSharpness{16};
    bool      mFullBright{false};

protected:
    ID mDiffuseTextureID{},
        mSpecularTextureID{};
    float mSpecularStrength{0.0f};
};

#endif // MATERIAL_H
