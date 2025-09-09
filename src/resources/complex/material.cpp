#include "material.hpp"

rid_t Material::GetDiffuseTexture() const
{ return m_DiffuseTextureID; }

void Material::SetDiffuseTexture(rid_t texture_id)
{ m_DiffuseTextureID = texture_id; }

rid_t Material::GetSpecularTexture() const
{ return m_SpecularTextureID; }

void Material::SetSpecularTexture(rid_t texture_id)
{ m_SpecularTextureID = texture_id; }
