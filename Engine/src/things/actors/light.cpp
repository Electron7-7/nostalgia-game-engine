#include "light.hpp"
#include "theatre/parser/thing_data.hpp"
#include "managers/theatre_manager.hpp"
#ifdef DEBUGGING
#   define TRUE_IF_DEBUGGING(boolean) boolean = true
#else  // !DEBUGGING
#   define TRUE_IF_DEBUGGING(boolean) boolean = false
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

    // The light's debug mesh should only be visible by default on debug builds
    TRUE_IF_DEBUGGING(mVisible);

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
    if(mMeshInstanceID == ID::Invalid) { // the debug mesh/material shouldn't override a manually specificed one
        { mEnabled = !mEnabled; }
        ID mat_id = g_pTheatreManager->CreateThing({
            mName + "_DebugMaterial",
            ThingType::Material,
            UniqueID::Generate(),
            {
                {UniqueID::Reserved::i_LightDebug, "DiffuseTexture"},
                {true, "FullBright"},
            }
        });
        mMeshInstanceID = g_pTheatreManager->CreateThing({
            mName + "_DebugMeshInstance",
            ThingType::MeshInstance,
            UniqueID::Generate(),
            {
                {UniqueID::Reserved::m_Cube, "Mesh"},
                {mat_id, "Material"}
            }
        });
    }
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

bool PointLight::IncrementIndex()
{
    if(!mEnabled || light_t::sPointCount >= MAX_POINT)
        { return false; }
    mIndex = light_t::sPointCount++;
    return true;
}

int SpotLight::GetCount()
{ return light_t::sSpotCount;}

void SpotLight::SetVariables(const ThingData& data)
{
    light_t::SetVariables(data);
    data.GetVariable(mSpotAngle, "Angle");
    data.GetVariable(mSpotAngle, "SpotAngle");
    data.GetVariable(mSpotAngleFade, "AngleFade");
    data.GetVariable(mSpotAngleFade, "SpotAngleFade");
}

ThingData SpotLight::GetVariables() const
{
    ThingData data{light_t::GetVariables()};
    data.AddVariable(mSpotAngle, "Angle");
    data.AddVariable(mSpotAngle, "SpotAngle");
    data.AddVariable(mSpotAngleFade, "AngleFade");
    data.AddVariable(mSpotAngleFade, "SpotAngleFade");
    return data;
}

bool SpotLight::IncrementIndex()
{
    if(!mEnabled || light_t::sSpotCount >= MAX_SPOT)
        { return false; }
    mIndex = light_t::sSpotCount++;
    return true;
}

int DirectionalLight::GetCount()
{ return light_t::sDirectionalCount;}

void DirectionalLight::SetVariables(const ThingData& data)
{
    SetEuler(glm::vec3(-90.0f, 0.0f, 0.0f), true);
    light_t::SetVariables(data);
    mVisible = false;
}

ThingData DirectionalLight::GetVariables() const
{
    ThingData data{light_t::GetVariables()};
    data.AddVariable(false, "Visible");
    return data;
}

bool DirectionalLight::IncrementIndex()
{
    if(!mEnabled || light_t::sDirectionalCount >= MAX_DIRECTIONAL)
        { return false; }
    mIndex = light_t::sDirectionalCount++;
    return true;
}
