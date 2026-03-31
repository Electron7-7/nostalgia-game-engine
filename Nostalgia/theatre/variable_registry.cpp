#include "./variable_registry.hpp"
#include "rendering/texture_buffer.hpp"
#include "things/thinkers/3d/collider_3d.hpp"
#include "rendering/environment.hpp"

RMutex VariableRegistry::m_sEnumsMutex{};
VariableRegistry::Enums VariableRegistry::m_sEnums{};

VariableRegistry::VariableRegistry() noexcept = default;

Farg<VariableRegistry::Enums> VariableRegistry::GetRegisteredEnums()
{ return m_sEnums; }

void VariableRegistry::RegisterEngineEnums()
{
    m_sEnums["Static"]                 = MotionType::Static;
    m_sEnums["Dynamic"]                = MotionType::Dynamic;
    m_sEnums["Kinematic"]              = MotionType::Kinematic;
    m_sEnums["Box"]                    = ShapeType::Box;
    m_sEnums["Capsule"]                = ShapeType::Capsule;
    m_sEnums["Cylinder"]               = ShapeType::Cylinder;
    m_sEnums["Sphere"]                 = ShapeType::Sphere;
    m_sEnums["2DTexture"]              = TextureType::TEXTURE_TYPE_2D;
    m_sEnums["CubeMapTexture"]         = TextureType::TEXTURE_TYPE_CUBE;
    m_sEnums["CustomColor"]            = Environment::BG_CUSTOM_COLOR;
    m_sEnums["ClearColor"]             = Environment::BG_CLEAR_COLOR;
    m_sEnums["Skybox"]                 = Environment::BG_SKYBOX;
    m_sEnums["SamplerNoFilter"]        = SAMPLER_FILTER_NONE;
    m_sEnums["SamplerFilterLinear"]    = SAMPLER_FILTER_LINEAR;
    m_sEnums["SamplerFilterNearest"]   = SAMPLER_FILTER_NEAREST;
    m_sEnums["SamplerRepeat"]          = SAMPLER_REPEAT_MODE_REPEAT;
    m_sEnums["SamplerRepeatMirrored"]  = SAMPLER_REPEAT_MODE_MIRRORED_REPEAT;
    m_sEnums["SamplerClampEdge"]       = SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE;
    m_sEnums["SamplerClampBorder"]     = SAMPLER_REPEAT_MODE_CLAMP_TO_BORDER;
    m_sEnums["SamplerMirrorClampEdge"] = SAMPLER_REPEAT_MODE_MIRROR_CLAMP_TO_EDGE;
}
