#ifndef MATERIAL_H
#define MATERIAL_H

#include <Nostalgia/things/resources/resource.hpp>
#include <Nostalgia/things/resources/texture.hpp>

class Material : public Resource
{
public:
	SET_SUPER(Resource)
	SET_TYPEID(ThingType::Material)
    virtual SIG_SET_VARIABLES override;
    virtual SIG_GET_VARIABLES override;

    Shared<Texture> DiffuseTexture() const;
    void DiffuseTexture(Shared<Texture> Texture);

    Shared<Texture> SpecularTexture() const;
    void SpecularTexture(Shared<Texture> Texture);

    float SpecularStrength() const;
    void SpecularStrength(float);

    glm::vec3 mColor{1.0f};
    float     mAlpha{1.0f};
    int       mSpecularSharpness{16};
    bool      mFullBright{false};

protected:
    Shared<Texture> m_pDiffuseTexture{MakeShared<Texture>()},
        m_pSpecularTexture{MakeShared<Texture>()};
    float mSpecularStrength{0.0f};
};

#endif // MATERIAL_H
