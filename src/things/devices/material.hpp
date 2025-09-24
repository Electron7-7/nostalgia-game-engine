#ifndef MATERIAL_H
#define MATERIAL_H

#include "../resources/resource.hpp"

#include <glm/glm.hpp>

class Material : public Resource
{
public:
    virtual void SetupVariables(const data_t&);

    id_t GetDiffuseTexture() const;
    void SetDiffuseTexture(id_t TextureID);

    id_t GetSpecularTexture() const;
    void SetSpecularTexture(id_t TextureID);

    glm::vec3 m_Color = glm::vec3(1.0f);
    float     m_Alpha = 1.0f;
    int       m_SpecularSharpness = 16;
    float     m_SpecularStrength = 0.0f;
    bool      m_DontUseTexture = false;
    glm::vec4 m_DebugHighlight = glm::vec4(0.9f, 0.2f, 1.0f, 0.0f);

private:
    id_t m_DiffuseTextureID  = ID::None;
    id_t m_SpecularTextureID = ID::None;
};

#endif // MATERIAL_H
