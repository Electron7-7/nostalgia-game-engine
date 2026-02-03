#include "material.hpp"

using namespace TheatreFile;

void Material::SetVariables(Farg<ThingData> data)
{
    Resource::SetVariables(data);

    data.get_variable(mDiffuseTextureID, "DiffuseTexture");
    data.get_variable(mSpecularTextureID, "SpecularTexture");
    data.get_variable(mColor, "Color");
    data.get_variable(mAlpha, "Alpha");
    data.get_variable(mSpecularSharpness, "SpecularSharpness");
    data.get_variable(mSpecularStrength, "SpecularStrength");
    data.get_variable(mFullBright, "FullBright", "mat_fullbright");
}

Shared<ThingData> Material::GetVariables() const
{
    Shared<ThingData> data{Resource::GetVariables()};

    data->set_variable(mDiffuseTextureID, "DiffuseTexture");
    data->set_variable(mSpecularTextureID, "SpecularTexture");
    data->set_variable(mColor, "Color");
    data->set_variable(mAlpha, "Alpha");
    data->set_variable(mSpecularSharpness, "SpecularSharpness");
    data->set_variable(mSpecularStrength, "SpecularStrength");
    data->set_variable(mFullBright, "FullBright");

    return data;
}

ID Material::DiffuseTextureID() const
{ return mDiffuseTextureID; }

void Material::DiffuseTextureID(ID texture_id)
{ mDiffuseTextureID = texture_id; }

ID Material::SpecularTextureID() const
{ return mSpecularTextureID; }

void Material::SpecularTextureID(ID texture_id)
{ mSpecularTextureID = texture_id; }

float Material::SpecularStrength() const
{ return mSpecularStrength; }

void Material::SpecularStrength(float inStrength)
{ mSpecularStrength = inStrength; }
