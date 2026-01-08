#include "material.hpp"
#include "core/uid.hpp"
#include "theatre/parser/thing_data.hpp"

void Material::SetVariables(Farg<ThingData> data)
{
    Resource::SetVariables(data);

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
    data.GetVariable(mDontUseTexture, "NoTexture", "NoTextures");
    data.GetVariable(mFullBright, "FullBright", "mat_fullbright");
}

Shared<ThingData> Material::GetVariables() const
{
    Shared<ThingData> data{Resource::GetVariables()};

    data->AddVariable(mDiffuseTextureID, "DiffuseTexture");
    data->AddVariable(mSpecularTextureID, "SpecularTexture");
    data->AddVariable(mColor, "Color");
    data->AddVariable(mAlpha, "Alpha");
    data->AddVariable(mSpecularSharpness, "SpecularSharpness");
    data->AddVariable(mSpecularStrength, "SpecularStrength");
    data->AddVariable(mDontUseTexture, "UseVertexColors");
    data->AddVariable(mDontUseTexture, "OnlyUseVertexColors");
    data->AddVariable(mDontUseTexture, "NoTexture");
    data->AddVariable(mFullBright, "FullBright");

    return data;
}

ID Material::DiffuseTextureID() const
{
    return (mDiffuseTextureID.invalid())
        ? UID::t_Missing
        : mDiffuseTextureID;
}

void Material::DiffuseTextureID(ID texture_id)
{ mDiffuseTextureID = texture_id; }

ID Material::SpecularTextureID() const
{
    return (mSpecularTextureID.invalid())
        ? UID::t_Missing
        : mSpecularTextureID;
}

void Material::SpecularTextureID(ID texture_id)
{ mSpecularTextureID = texture_id; }
