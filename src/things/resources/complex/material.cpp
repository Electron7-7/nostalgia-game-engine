#include "material.hpp"

void Material::SetupVariables(const data_t& data)
{
    // Resource::SetupVariables(data); // This currently doesn't do anything

    data.GetTheatreRef(m_DiffuseTextureID, "DiffuseTexture");
    data.GetTheatreRef(m_SpecularTextureID, "SpecularTexture");
}

id_t Material::GetDiffuseTexture() const
{ return m_DiffuseTextureID; }

void Material::SetDiffuseTexture(id_t texture_id)
{ m_DiffuseTextureID = texture_id; }

id_t Material::GetSpecularTexture() const
{ return m_SpecularTextureID; }

void Material::SetSpecularTexture(id_t texture_id)
{ m_SpecularTextureID = texture_id; }
