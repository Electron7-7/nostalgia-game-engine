#include "light.hpp"
#include "theatre_parser/thing_data.hpp"
#ifdef DEBUGGING
#   include "managers/theatre_manager.hpp"
#endif // DEBUGGING

int light_t::sPointCount       = 0;
int light_t::sSpotCount        = 0;
int light_t::sDirectionalCount = 0;

void light_t::ClearCounts()
{
    light_t::sPointCount       = 0;
    light_t::sSpotCount        = 0;
    light_t::sDirectionalCount = 0;
}

void light_t::SetVariables(const ThingData& data)
{
    mScale = glm::vec3(0.1f);

    Actor::SetVariables(data);

    data.GetVariable(mColor, "Color");
    data.GetVariable(mEnergy, "Energy");
    data.GetVariable(mSpecularStrength, "SpecularStrength");
    data.GetVariable(mAmbientStrength, "AmbientStrength");
    data.GetVariable(mAttenuation, "FadeIntensity");
    data.GetVariable(mAttenuation, "Attenuation");
    data.GetVariable(mRange, "Range");
    data.GetVariable(mEnabled, "LightVisible");
    data.GetVariable(mEnabled, "Enabled");
    if(data.GetVariable(mEnabled, "Disabled"))
        { mEnabled = !mEnabled; }

#pragma message("TODO: should the light's debug mesh be limited to the debug build, or should Lights be always invisible with a special variable for enabling their debug mesh?")
#ifdef DEBUGGING
    mVisible = true;
    if(mMeshInstanceID == ID::Invalid) { // the debug mesh/material shouldn't override a manually specificed one
        ID mat_id = TheatreManager::CreateThing({
            mName + "_DebugMaterial",
            ThingType::Material,
            UniqueIDs::Generate(),
            {
                {UniqueIDs::Reserved::i_LightDebug, "DiffuseTexture"},
                {true, "FullBright"},
            }
        });
        mMeshInstanceID = TheatreManager::CreateThing({
            mName + "_DebugMeshInstance",
            ThingType::MeshInstance,
            UniqueIDs::Generate(),
            {
                {UniqueIDs::Reserved::m_Cube, "Mesh"},
                {mat_id, "Material"}
            }
        });
    }
#else  // !DEBUGGING
    mVisible = false;
#endif // DEBUGGING
}

ThingData light_t::GetVariables() const
{
    ThingData data{Actor::GetVariables()};

    data.AddVariable(mColor, "Color");
    data.AddVariable(mEnergy, "Energy");
    data.AddVariable(mSpecularStrength, "SpecularStrength");
    data.AddVariable(mAmbientStrength, "AmbientStrength");
    data.AddVariable(mAttenuation, "FadeIntensity");
    data.AddVariable(mAttenuation, "Attenuation");
    data.AddVariable(mRange, "Range");
    data.AddVariable(mEnabled, "LightVisible");
    data.AddVariable(mEnabled, "Enabled");
    data.AddVariable(mVisible, "Visible");
#ifndef DEBUGGING // See above pragma message
    data.RemoveVariable("Mesh");
    data.RemoveVariable("Material");
#endif

    return data;
}

int light_t::Index() const
{ return mIndex; }

int PointLight::GetCount()
{ return light_t::sPointCount;}

LightType PointLight::Type() const
{ return LightType::POINT; }

bool PointLight::IncrementIndex()
{
    if(light_t::sPointCount >= MAX_POINT)
        { return false; }
    mIndex = light_t::sPointCount++;
    return true;
}

int SpotLight::GetCount()
{ return light_t::sSpotCount;}

void SpotLight::SetupVariables(const ThingData& data)
{
    light_t::SetVariables(data);
    data.GetVariable(mSpotAngle, "Angle");
    data.GetVariable(mSpotAngle, "SpotAngle");
    data.GetVariable(mSpotAngleFade, "AngleFade");
    data.GetVariable(mSpotAngleFade, "SpotAngleFade");
}

LightType SpotLight::Type() const
{ return LightType::SPOT; }

bool SpotLight::IncrementIndex()
{
    if(light_t::sSpotCount >= MAX_SPOT)
        { return false; }
    mIndex = light_t::sSpotCount++;
    return true;
}

int DirectionalLight::GetCount()
{ return light_t::sDirectionalCount;}

void DirectionalLight::SetupVariables(const ThingData& data)
{
    SetEuler(glm::vec3(-90.0f, 0.0f, 0.0f), true);
    mVisible = false;
    light_t::SetVariables(data);
}

LightType DirectionalLight::Type() const
{ return LightType::DIRECTIONAL; }

bool DirectionalLight::IncrementIndex()
{
    if(light_t::sDirectionalCount >= MAX_DIRECTIONAL)
        { return false; }
    mIndex = light_t::sDirectionalCount++;
    return true;
}
