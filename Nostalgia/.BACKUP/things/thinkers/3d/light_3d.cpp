#include "./light_3d.hpp"
#include "things/thing_data.hpp"
#include "things/thing_factory.hpp"
#include "settings/engine.hpp"
#include "theatre/theatre.hpp"
#ifdef NOSTALGIA_DEBUGGING
#   define TRUE_IF_DEBUGGING(boolean) boolean = true
#else  // !NOSTALGIA_DEBUGGING
#   define TRUE_IF_DEBUGGING(boolean) boolean = false
#endif // NOSTALGIA_DEBUGGING

using namespace TheatreFile;

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
    Super::Ready();
}

void Light3D::InitVariables()
{
    Super::InitVariables();
}

void Light3D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mColor, "Color");
    data.get_variable(mEnergy, "Energy");
    data.get_variable(mSpecularStrength, "SpecularStrength");
    data.get_variable(mAmbientStrength, "AmbientStrength");
    data.get_variable(mAttenuation, "FadeIntensity");
    data.get_variable(mAttenuation, "Attenuation");
    data.get_variable(mRange, "Range");
    data.get_variable(mEnabled, "LightVisible", "Enabled");
    if(data.get_variable(mEnabled, "LightDisabled", "Disabled") == OK)
        { mEnabled = !mEnabled; }
}

Shared<ThingData> Light3D::GetVariables() const
{
    Shared<ThingData> data{Super::GetVariables()};

    data->set_variable(mColor, "Color");
    data->set_variable(mEnergy, "Energy");
    data->set_variable(mSpecularStrength, "SpecularStrength");
    data->set_variable(mAmbientStrength, "AmbientStrength");
    data->set_variable(mAttenuation, "FadeIntensity");
    data->set_variable(mAttenuation, "Attenuation");
    data->set_variable(mRange, "Range");
    data->set_variable(mEnabled, "LightVisible");
    data->set_variable(mVisible, "Visible");

    return data;
}

int Light3D::Index() const
{ return mIndex; }

void PointLight3D::InitVariables()
{
    Super::InitVariables();
}

int PointLight3D::GetCount()
{ return Super::sPointCount;}

bool PointLight3D::IncrementIndex()
{
    if(!mEnabled || Super::sPointCount >= MAX_POINT)
        { return false; }
    mIndex = Super::sPointCount++;
    return true;
}

int SpotLight3D::GetCount()
{ return Super::sSpotCount;}

void SpotLight3D::InitVariables()
{
    Super::InitVariables();
}

void SpotLight3D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);
    data.get_variable(mSpotAngle, "SpotAngle");
    data.get_variable(mSpotAngleFade, "SpotAngleFade");
}

Shared<ThingData> SpotLight3D::GetVariables() const
{
    Shared<ThingData> data{Super::GetVariables()};
    data->set_variable(mSpotAngle, "SpotAngle");
    data->set_variable(mSpotAngleFade, "SpotAngleFade");
    return data;
}

bool SpotLight3D::IncrementIndex()
{
    if(!mEnabled || Super::sSpotCount >= MAX_SPOT)
        { return false; }
    mIndex = Super::sSpotCount++;
    return true;
}

int DirectionalLight3D::GetCount()
{ return Super::sDirectionalCount;}

void DirectionalLight3D::InitVariables()
{
    Super::InitVariables();
}

void DirectionalLight3D::SetVariables(Farg<ThingData> data)
{
    SetRotationDegrees({-90.0f, 0.0f, 0.0f});
    Super::SetVariables(data);
    mVisible = false;
    data.get_variable(mVisible, "ShowDirectionalLightDebugMesh", "ShowLightMeshOverride");
}

Shared<ThingData> DirectionalLight3D::GetVariables() const
{
    Shared<ThingData> data{Super::GetVariables()};
    if(mVisible)
        { data->set_variable(mVisible, "ShowDirectionalLightDebugMesh"); }
    return data;
}

bool DirectionalLight3D::IncrementIndex()
{
    if(!mEnabled || Super::sDirectionalCount >= MAX_DIRECTIONAL)
        { return false; }
    mIndex = Light3D::sDirectionalCount++;
    return true;
}
