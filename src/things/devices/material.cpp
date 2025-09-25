#include "material.hpp"
#include "theatre/data_t.hpp"

void Material::SetupVariables(const data_t& data)
{
    Device::SetupVariables(data);

    data.GetTheatreRef(m_DiffuseTextureID, "DiffuseTexture");
    data.GetTheatreRef(m_SpecularTextureID, "SpecularTexture");
    data.GetNumber(m_Color, "Color");
    data.GetNumber(m_Alpha, "Alpha");
    data.GetNumber(m_SpecularSharpness, "SpecularSharpness");
    // TODO: Pick a better default value?
    if(!data.GetNumber(m_SpecularStrength, "SpecularStrength") && m_SpecularTextureID != ID::None)
        { m_SpecularStrength = 1.0f; }
    data.GetBool(m_DontUseTexture, "UseVertexColors");
    data.GetBool(m_DontUseTexture, "OnlyUseVertexColors");
    data.GetBool(m_DontUseTexture, "NoTexture");
}

id_t Material::GetDiffuseTexture() const
{ return m_DiffuseTextureID; }

void Material::SetDiffuseTexture(id_t texture_id)
{ m_DiffuseTextureID = texture_id; }

id_t Material::GetSpecularTexture() const
{ return m_SpecularTextureID; }

void Material::SetSpecularTexture(id_t texture_id)
{ m_SpecularTextureID = texture_id; }
