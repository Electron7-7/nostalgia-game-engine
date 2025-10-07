#include "material.hpp"
#include "theatre_parser/thing_data.hpp"

void Material::SetupVariables(const ThingData& data)
{
    Device::SetupVariables(data);

    data.GetReference(mDiffuseTextureID, "DiffuseTexture");
    data.GetReference(mSpecularTextureID, "SpecularTexture");
    data.GetNumber(mColor, "Color");
    data.GetNumber(mAlpha, "Alpha");
    data.GetNumber(mSpecularSharpness, "SpecularSharpness");
    // TODO: Pick a better default value?
    if(!data.GetNumber(mSpecularStrength, "SpecularStrength") && mSpecularTextureID != ID::None)
        { mSpecularStrength = 1.0f; }
    data.GetBool(mDontUseTexture, "UseVertexColors");
    data.GetBool(mDontUseTexture, "OnlyUseVertexColors");
    data.GetBool(mDontUseTexture, "NoTexture");
}

ID Material::GetDiffuseTexture() const
{ return mDiffuseTextureID; }

void Material::SetDiffuseTexture(ID texture_id)
{ mDiffuseTextureID = texture_id; }

ID Material::GetSpecularTexture() const
{ return mSpecularTextureID; }

void Material::SetSpecularTexture(ID texture_id)
{ mSpecularTextureID = texture_id; }
