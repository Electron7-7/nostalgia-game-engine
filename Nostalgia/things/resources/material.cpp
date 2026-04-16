#include "./material.hpp"
#include "./texture.hpp"
#include "things/thing_data.hpp"
#include "theatre/theatre.hpp"

using namespace TheatreFile;

void Material::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(m_pDiffuseTexture, "DiffuseTexture");
    data.get_variable(m_pSpecularTexture, "SpecularTexture");
    data.get_variable(mColor, "Color");
    data.get_variable(mAlpha, "Alpha");
    data.get_variable(mSpecularSharpness, "SpecularSharpness");
    data.get_variable(mSpecularStrength, "SpecularStrength");
    data.get_variable(mFullBright, "FullBright");
}

Shared<ThingData> Material::GetVariables() const
{
    Shared<ThingData> data{Super::GetVariables()};

    data->set_variable(m_pDiffuseTexture, "DiffuseTexture");
    data->set_variable(m_pSpecularTexture, "SpecularTexture");
    data->set_variable(mColor, "Color");
    data->set_variable(mAlpha, "Alpha");
    data->set_variable(mSpecularSharpness, "SpecularSharpness");
    data->set_variable(mSpecularStrength, "SpecularStrength");
    data->set_variable(mFullBright, "FullBright");

    return data;
}

Shared<Texture> Material::DiffuseTexture() const
{ return m_pDiffuseTexture; }

void Material::DiffuseTexture(Shared<Texture> inTexture)
{ m_pDiffuseTexture = inTexture; }

Shared<Texture> Material::SpecularTexture() const
{ return m_pSpecularTexture; }

void Material::SpecularTexture(Shared<Texture> inTexture)
{ m_pSpecularTexture = inTexture; }

float Material::SpecularStrength() const
{ return mSpecularStrength; }

void Material::SpecularStrength(float inStrength)
{ mSpecularStrength = inStrength; }
