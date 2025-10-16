#include "material.hpp"
#include "theatre_parser/thing_data.hpp"

void Material::SetVariables(const ThingData& data)
{
    Device::SetVariables(data);

    data.GetVariable(mDiffuseTextureID, "DiffuseTexture");
    data.GetVariable(mSpecularTextureID, "SpecularTexture");
    data.GetVariable(mColor, "Color");
    data.GetVariable(mAlpha, "Alpha");
    data.GetVariable(mSpecularSharpness, "SpecularSharpness");
    // TODO: Pick a better default value?
    if(!data.GetVariable(mSpecularStrength, "SpecularStrength") && mSpecularTextureID != ID::Invalid)
        { mSpecularStrength = 1.0f; }
    data.GetVariable(mDontUseTexture, "UseVertexColors");
    data.GetVariable(mDontUseTexture, "OnlyUseVertexColors");
    data.GetVariable(mDontUseTexture, "NoTexture");
}

ThingData Material::GetVariables() const
{
    ThingData data{Device::GetVariables()};

    data.AddVariable(mDiffuseTextureID, "DiffuseTexture");
    data.AddVariable(mSpecularTextureID, "SpecularTexture");
    data.AddVariable(mColor, "Color");
    data.AddVariable(mAlpha, "Alpha");
    data.AddVariable(mSpecularSharpness, "SpecularSharpness");
    data.AddVariable(mSpecularStrength, "SpecularStrength");
    data.AddVariable(mDontUseTexture, "UseVertexColors");
    data.AddVariable(mDontUseTexture, "OnlyUseVertexColors");
    data.AddVariable(mDontUseTexture, "NoTexture");

    return data;
}

ID Material::GetDiffuseTexture() const
{ return mDiffuseTextureID; }

void Material::SetDiffuseTexture(ID texture_id)
{ mDiffuseTextureID = texture_id; }

ID Material::GetSpecularTexture() const
{ return mSpecularTextureID; }

void Material::SetSpecularTexture(ID texture_id)
{ mSpecularTextureID = texture_id; }
