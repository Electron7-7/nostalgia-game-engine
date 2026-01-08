#include "light3d.hpp"
#include "core/uid.hpp"
#include "settings/engine.hpp"
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

void light_t::Ready()
{
    // the debug mesh/material shouldn't override a manually specificed one
    if(mDebugMeshInstanceID.invalid() and Settings::Engine::IsEditorHint)
    {
        ID mat_id{g_pTheatreManager->CreateThing({
            mName + "-debug-material",
            ThingType::Material,
            UID::Generate(),
            {
                {mColor, "Color"},
                {UID::t_LightDebug, "DiffuseTexture"},
                {true, "FullBright"}
            }
        })};
        mDebugMeshInstanceID = g_pTheatreManager->CreateThing({
            mName + "-debug-mesh-instance",
            ThingType::MeshInstance3D,
            UID::Generate(),
            {
                {UID::m_Cube, "Mesh"},
                {mat_id, "MaterialOverride"}
            }
        });
    }
}

void light_t::SetVariables(Farg<ThingData> data)
{
    mScale = glm::vec3{0.1f};

    Actor3D::SetVariables(data);

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
}

Shared<ThingData> light_t::GetVariables() const
{
    Shared<ThingData> data{Actor3D::GetVariables()};

    data->AddVariable(mColor, "Color");
    data->AddVariable(mEnergy, "Energy");
    data->AddVariable(mSpecularStrength, "SpecularStrength");
    data->AddVariable(mAmbientStrength, "AmbientStrength");
    data->AddVariable(mAttenuation, "FadeIntensity");
    data->AddVariable(mAttenuation, "Attenuation");
    data->AddVariable(mRange, "Range");
    data->AddVariable(mEnabled, "LightVisible");
    data->AddVariable(mEnabled, "Enabled");
    data->AddVariable(mVisible, "Visible");

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

void SpotLight::SetVariables(Farg<ThingData> data)
{
    light_t::SetVariables(data);
    data.GetVariable(mSpotAngle, "Angle");
    data.GetVariable(mSpotAngle, "SpotAngle");
    data.GetVariable(mSpotAngleFade, "AngleFade");
    data.GetVariable(mSpotAngleFade, "SpotAngleFade");
}

Shared<ThingData> SpotLight::GetVariables() const
{
    Shared<ThingData> data{light_t::GetVariables()};
    data->AddVariable(mSpotAngle, "Angle");
    data->AddVariable(mSpotAngle, "SpotAngle");
    data->AddVariable(mSpotAngleFade, "AngleFade");
    data->AddVariable(mSpotAngleFade, "SpotAngleFade");
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

void DirectionalLight::SetVariables(Farg<ThingData> data)
{
    RotationDegrees({-90.0f, 0.0f, 0.0f});
    light_t::SetVariables(data);
    mVisible = false;
}

Shared<ThingData> DirectionalLight::GetVariables() const
{
    Shared<ThingData> data{light_t::GetVariables()};
    data->AddVariable(false, "Visible");
    return data;
}

bool DirectionalLight::IncrementIndex()
{
    if(!mEnabled || light_t::sDirectionalCount >= MAX_DIRECTIONAL)
        { return false; }
    mIndex = light_t::sDirectionalCount++;
    return true;
}
