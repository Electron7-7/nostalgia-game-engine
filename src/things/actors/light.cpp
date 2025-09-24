#include "light.hpp"
#include "debug.hpp"
#include "embedded/models.hpp"
#include "embedded/images.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/data_t.hpp"
#include "theatre/variable_t.hpp"

int light_t::s_PointCount       = 0;
int light_t::s_SpotCount        = 0;
int light_t::s_DirectionalCount = 0;

void light_t::ClearCounts()
{
    light_t::s_PointCount       = 0;
    light_t::s_SpotCount        = 0;
    light_t::s_DirectionalCount = 0;
}

void light_t::SetupVariables(const data_t& data)
{
    Scale() = glm::vec3(0.1f);

    Actor::SetupVariables(data);

    data.GetNumber(m_Color, "Color");
    data.GetNumber(m_Energy, "Energy");
    data.GetNumber(m_SpecularStrength, "SpecularStrength");
    data.GetNumber(m_AmbientStrength, "AmbientStrength");
    data.GetNumber(m_Attenuation, "FadeIntensity");
    data.GetNumber(m_Attenuation, "Attenuation");
    data.GetNumber(m_Range, "Range");
    data.GetBool(m_Enabled, "LightVisible");
    data.GetBool(m_Enabled, "Enabled");
    if(data.GetBool(m_Enabled, "Disabled"))
        { m_Enabled = !m_Enabled; }

    NOTDEBUG(m_Visible = false;)
    DEBUG(if(m_MeshInstanceID == ID::None) { // the debug mesh/material shouldn't override a manually specificed one
        id_t mat_id = TheatreManager::CreateThing(data_t{
            m_Name + "'s Debug Material",
            Type::Material,
            ID::GetNewID(),
            {
                variable_t{"DiffuseTexture", std::to_string(Image::ID::LightDebugging), VariableType::TheatreRef},
                variable_t{"FullBright", "true", VariableType::Bool},
            }
        });
        m_MeshInstanceID = TheatreManager::CreateThing(data_t{
            m_Name + "'s Debug MeshInstance",
            Type::MeshInstance,
            ID::GetNewID(),
            {
                variable_t{"Mesh", std::to_string(Model::ID::Cube), VariableType::TheatreRef},
                variable_t{"Material", std::to_string(mat_id), VariableType::TheatreRef}
            }
        });
    })
}

int light_t::Index() const
{ return m_Index; }

int PointLight::GetCount()
{ return light_t::s_PointCount;}

LightType PointLight::Type() const
{ return LightType::POINT; }

bool PointLight::IncrementIndex()
{
    if(light_t::s_PointCount >= MAX_POINT)
        { return false; }
    m_Index = light_t::s_PointCount++;
    return true;
}

int SpotLight::GetCount()
{ return light_t::s_SpotCount;}

void SpotLight::SetupVariables(const data_t& data)
{
    light_t::SetupVariables(data);
    data.GetNumber(m_SpotAngle, "Angle");
    data.GetNumber(m_SpotAngleFade, "AngleFade");
    data.GetNumber(m_SpotAngleFade, "AngleFadeIntensity");
}

LightType SpotLight::Type() const
{ return LightType::SPOT; }

bool SpotLight::IncrementIndex()
{
    if(light_t::s_SpotCount >= MAX_SPOT)
        { return false; }
    m_Index = light_t::s_SpotCount++;
    return true;
}

int DirectionalLight::GetCount()
{ return light_t::s_DirectionalCount;}

void DirectionalLight::SetupVariables(const data_t& data)
{
    Euler(glm::vec3(-90.0f, 0.0f, 0.0f), true);
    m_Visible = false;
    light_t::SetupVariables(data);
}

LightType DirectionalLight::Type() const
{ return LightType::DIRECTIONAL; }

bool DirectionalLight::IncrementIndex()
{
    if(light_t::s_DirectionalCount >= MAX_DIRECTIONAL)
        { return false; }
    m_Index = light_t::s_DirectionalCount++;
    return true;
}
