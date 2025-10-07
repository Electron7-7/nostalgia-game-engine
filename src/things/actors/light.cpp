#include "light.hpp"
#include "debug.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre_parser/thing_data.hpp"
#include "theatre_parser/thing_variable.hpp"

int light_t::sPointCount       = 0;
int light_t::sSpotCount        = 0;
int light_t::sDirectionalCount = 0;

void light_t::ClearCounts()
{
    light_t::sPointCount       = 0;
    light_t::sSpotCount        = 0;
    light_t::sDirectionalCount = 0;
}

void light_t::SetupVariables(const ThingData& data)
{
    Scale() = glm::vec3(0.1f);

    Actor::SetupVariables(data);

    data.GetNumber(mColor, "Color");
    data.GetNumber(mEnergy, "Energy");
    data.GetNumber(mSpecularStrength, "SpecularStrength");
    data.GetNumber(mAmbientStrength, "AmbientStrength");
    data.GetNumber(mAttenuation, "FadeIntensity");
    data.GetNumber(mAttenuation, "Attenuation");
    data.GetNumber(mRange, "Range");
    data.GetBool(mEnabled, "LightVisible");
    data.GetBool(mEnabled, "Enabled");
    if(data.GetBool(mEnabled, "Disabled"))
        { mEnabled = !mEnabled; }

    NOTDEBUG(mVisible = false;)
    DEBUG(if(mMeshInstanceID == ID::None) { // the debug mesh/material shouldn't override a manually specificed one
        id_t mat_id = TheatreManager::CreateThing(ThingData{
            mName + "'s Debug Mat",
            ThingType::Material,
            UniqueIDs::Generate(),
            {
                ThingVar{"DiffuseTexture", std::to_string(UniqueIDs::Reserved::i_LightDebug), ThingVar::eReference},
                ThingVar{"FullBright", "true", ThingVar::eBool},
            }
        });
        mMeshInstanceID = TheatreManager::CreateThing(ThingData{
            mName + "'s Debug MeshInst",
            ThingType::MeshInstance,
            UniqueIDs::Generate(),
            {
                ThingVar{"Mesh", std::to_string(UniqueIDs::Reserved::m_Cube), ThingVar::eReference},
                ThingVar{"Material", std::to_string(mat_id), ThingVar::eReference}
            }
        });
    })
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
    light_t::SetupVariables(data);
    data.GetNumber(mSpotAngle, "Angle");
    data.GetNumber(mSpotAngle, "SpotAngle");
    data.GetNumber(mSpotAngleFade, "AngleFade");
    data.GetNumber(mSpotAngleFade, "SpotAngleFade");
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
    Euler(glm::vec3(-90.0f, 0.0f, 0.0f), true);
    mVisible = false;
    light_t::SetupVariables(data);
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
