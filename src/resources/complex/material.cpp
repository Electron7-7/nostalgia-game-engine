#include "material.hpp"


id_t Material::GetDiffuseTexture() const
{ return m_DiffuseTextureID; }

void Material::SetDiffuseTexture(id_t texture_id)
{ m_DiffuseTextureID = texture_id; }

id_t Material::GetSpecularTexture() const
{ return m_SpecularTextureID; }

void Material::SetSpecularTexture(id_t texture_id)
{ m_SpecularTextureID = texture_id; }
