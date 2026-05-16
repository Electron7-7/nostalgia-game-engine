#ifndef MATERIAL_H
#define MATERIAL_H

#include <Nostalgia/things/resources/resource.hpp>
#include <Nostalgia/things/resources/texture.hpp>
#include <Nostalgia/things/thing_factory.hpp>

class Material : public Resource
{
public:
	THING_CLASS(Material, Resource)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

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
    Shared<Texture> m_pDiffuseTexture{ThingFactory::Invalid<Texture>()},
        m_pSpecularTexture{ThingFactory::Invalid<Texture>()};
    float mSpecularStrength{0.0f};
    SamplerState mSamplerState{};
};

#endif // MATERIAL_H
