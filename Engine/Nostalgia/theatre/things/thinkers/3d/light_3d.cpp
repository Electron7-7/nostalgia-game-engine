#include "light_3d.hpp"
#include "settings/engine.hpp"
#include "managers/theatre_manager.hpp"
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
    if(Settings::Engine::IsEditorHint // the debug mesh instance should only be visible in the editor
        and Type() != LightType::DIRECTIONAL) // the debug mesh instance shouldn't be visible for directional lights (yet)
    {
        ThingData mat_data{ThingType::Material, mName + "-debug-material"};
        mat_data.set_variable(mColor, "Color");
        mat_data.set_variable(UID::t_LightDebug, "DiffuseTexture");
        mat_data.set_variable(true, "FullBright");
        ID mat_id{g_pTheatreManager->CurrentTheatre()->CreateThing(mat_data)};

        ThingData mesh_inst_dat{ThingType::MeshInstance3D, mName + "-debug-mesh-instance"};
        mesh_inst_dat.set_variable(UID::m_Cube, "Mesh");
        mesh_inst_dat.set_variable(mat_id, "MaterialOverride");
        mesh_inst_dat.set_variable(glm::vec3{0.1f}, "Scale");
        mesh_inst_dat.set_variable(mUID, "Parent");

        my_theatre()->SetParent(my_theatre()->CreateThing(mesh_inst_dat), mUID);
    }
}

void Light3D::SetVariables(Farg<ThingData> data)
{
    Visual3D::SetVariables(data);

    data.get_variable(mColor, "Color");
    data.get_variable(mEnergy, "Energy");
    data.get_variable(mSpecularStrength, "SpecularStrength");
    data.get_variable(mAmbientStrength, "AmbientStrength");
    data.get_variable(mAttenuation, "FadeIntensity");
    data.get_variable(mAttenuation, "Attenuation");
    data.get_variable(mRange, "Range");
    data.get_variable(mEnabled, "LightVisible");
    data.get_variable(mEnabled, "Enabled");
    if(data.get_variable(mEnabled, "Disabled") == OK)
        { mEnabled = !mEnabled; }
}

Shared<ThingData> Light3D::GetVariables() const
{
    Shared<ThingData> data{Visual3D::GetVariables()};

    data->set_variable(mColor, "Color");
    data->set_variable(mEnergy, "Energy");
    data->set_variable(mSpecularStrength, "SpecularStrength");
    data->set_variable(mAmbientStrength, "AmbientStrength");
    data->set_variable(mAttenuation, "FadeIntensity");
    data->set_variable(mAttenuation, "Attenuation");
    data->set_variable(mRange, "Range");
    data->set_variable(mEnabled, "LightVisible");
    data->set_variable(mEnabled, "Enabled");
    data->set_variable(mVisible, "Visible");

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
    data.get_variable(mSpotAngle, "Angle");
    data.get_variable(mSpotAngle, "SpotAngle");
    data.get_variable(mSpotAngleFade, "AngleFade");
    data.get_variable(mSpotAngleFade, "SpotAngleFade");
}

Shared<ThingData> SpotLight3D::GetVariables() const
{
    Shared<ThingData> data{Light3D::GetVariables()};
    data->set_variable(mSpotAngle, "Angle");
    data->set_variable(mSpotAngle, "SpotAngle");
    data->set_variable(mSpotAngleFade, "AngleFade");
    data->set_variable(mSpotAngleFade, "SpotAngleFade");
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
    data->set_variable(false, "Visible");
    return data;
}

bool DirectionalLight3D::IncrementIndex()
{
    if(!mEnabled || Light3D::sDirectionalCount >= MAX_DIRECTIONAL)
        { return false; }
    mIndex = Light3D::sDirectionalCount++;
    return true;
}
