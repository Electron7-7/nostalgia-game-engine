#include "light_3d.hpp"
#include "core/uid.hpp"
#include "settings/engine.hpp"
#include "theatre/parser/thing_data.hpp"
#include "managers/theatre_manager.hpp"
#ifdef DEBUGGING
#   define TRUE_IF_DEBUGGING(boolean) boolean = true
#else  // !DEBUGGING
#   define TRUE_IF_DEBUGGING(boolean) boolean = false
#endif // DEBUGGING

int Light3D::sPointCount       = 0;
int Light3D::sSpotCount        = 0;
int Light3D::sDirectionalCount = 0;

void Light3D::ClearCounts()
{
    Light3D::sPointCount       = 0;
    Light3D::sSpotCount        = 0;
    Light3D::sDirectionalCount = 0;
}

void Light3D::Ready()
{
    if(mDebugMeshInstanceID.invalid() // the debug mesh instance shouldn't override a manually specificed one
        and Settings::Engine::IsEditorHint // the debug mesh instance should only be visible in the editor
        and Type() != LightType::DIRECTIONAL) // the debug mesh instance shouldn't be visible for directional lights (yet)
    {
        ID mat_id{g_pTheatreManager->CurrentTheatre()->CreateThing({
            mName + "-debug-material",
            ThingType::Material,
            UID::Generate(),
            {
                {mColor, "Color"},
                {UID::t_LightDebug, "DiffuseTexture"},
                {true, "FullBright"}
            }
        })};
        mDebugMeshInstanceID = g_pTheatreManager->CurrentTheatre()->CreateThing({
            mName + "-debug-mesh-instance",
            ThingType::MeshInstance3D,
            UID::Generate(),
            {
                {UID::m_Cube, "Mesh"},
                {mat_id, "MaterialOverride"},
                {glm::vec3{0.1f}, "Scale"},
                {mUID, "Parent"},
            }
        });
    }
}

void Light3D::SetVariables(Farg<ThingData> data)
{
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

Shared<ThingData> Light3D::GetVariables() const
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

int Light3D::Index() const
{ return mIndex; }

int PointLight3D::GetCount()
{ return Light3D::sPointCount;}

bool PointLight3D::IncrementIndex()
{
    if(!mEnabled || Light3D::sPointCount >= MAX_POINT)
        { return false; }
    mIndex = Light3D::sPointCount++;
    return true;
}

int SpotLight3D::GetCount()
{ return Light3D::sSpotCount;}

void SpotLight3D::SetVariables(Farg<ThingData> data)
{
    Light3D::SetVariables(data);
    data.GetVariable(mSpotAngle, "Angle");
    data.GetVariable(mSpotAngle, "SpotAngle");
    data.GetVariable(mSpotAngleFade, "AngleFade");
    data.GetVariable(mSpotAngleFade, "SpotAngleFade");
}

Shared<ThingData> SpotLight3D::GetVariables() const
{
    Shared<ThingData> data{Light3D::GetVariables()};
    data->AddVariable(mSpotAngle, "Angle");
    data->AddVariable(mSpotAngle, "SpotAngle");
    data->AddVariable(mSpotAngleFade, "AngleFade");
    data->AddVariable(mSpotAngleFade, "SpotAngleFade");
    return data;
}

bool SpotLight3D::IncrementIndex()
{
    if(!mEnabled || Light3D::sSpotCount >= MAX_SPOT)
        { return false; }
    mIndex = Light3D::sSpotCount++;
    return true;
}

int DirectionalLight3D::GetCount()
{ return Light3D::sDirectionalCount;}

void DirectionalLight3D::SetVariables(Farg<ThingData> data)
{
    SetRotationDegrees({-90.0f, 0.0f, 0.0f});
    Light3D::SetVariables(data);
    mVisible = false;
}

Shared<ThingData> DirectionalLight3D::GetVariables() const
{
    Shared<ThingData> data{Light3D::GetVariables()};
    data->AddVariable(false, "Visible");
    return data;
}

bool DirectionalLight3D::IncrementIndex()
{
    if(!mEnabled || Light3D::sDirectionalCount >= MAX_DIRECTIONAL)
        { return false; }
    mIndex = Light3D::sDirectionalCount++;
    return true;
}
