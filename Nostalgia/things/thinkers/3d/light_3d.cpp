#include "./light_3d.hpp"
#include "theatre/thing_data.hpp"
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
    Super::Ready();

    if(Settings::Engine::IsEditorHint // the debug mesh instance should only be visible in the editor
        and not DerivedFrom(ThingType::DirectionalLight3D)) // and not visible for directional lights
    {
        std::string mat_name{name() + "-debug-material"};
        ID mat_id{},
            mesh_inst_id{};
        if(!Theatre::Current()->ThingExists(mat_name))
        {
            ThingData mat_data{ThingType::Material, mat_name};
            mat_data.set_variable(mColor, "Color");
            mat_data.set_variable(UID::i_LightDebug, "DiffuseTexture");
            mat_data.set_variable(true, "FullBright");
            mat_id = g_pTheatreManager->Current()->CreateThing(mat_data);
        }
        else
            { mat_id = Theatre::Current()->GetThing(mat_name)->uid(); }

        std::string mesh_inst_name{name() + "-debug-mesh-instance"};
        if(!Theatre::Current()->ThingExists(mesh_inst_name))
        {
            ThingData mesh_inst_dat{ThingType::MeshInstance3D, mesh_inst_name};
            mesh_inst_dat.set_variable(UID::m_Cube, "Mesh");
            mesh_inst_dat.set_variable(mat_id, "MaterialOverride");
            mesh_inst_dat.set_variable(glm::vec3{0.1f}, "Scale");
            mesh_inst_dat.set_variable(uid(), "Parent");
            mesh_inst_id = Theatre::Current()->CreateThing(mesh_inst_dat);
        }
        else
            { mesh_inst_id = Theatre::Current()->GetThing(mesh_inst_name)->uid(); }

        Theatre::Current()->SetParent(mesh_inst_id, uid());
    }
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
