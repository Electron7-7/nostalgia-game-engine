#include "variable_registry.hpp"
#include "core/printing.hpp"
#include "core/uid.hpp"
#include "rendering/texture_buffer.hpp"
#include "theatre/things/thinkers/3d/collider_3d.hpp"
#include "rendering/environment.hpp"

VariableRegistry::Enums VariableRegistry::m_sEnums{};

VariableRegistry::VariableRegistry() noexcept = default;

Farg<VariableRegistry::References> VariableRegistry::GetRegisteredIDs() const
{ return mReferences; }

Farg<VariableRegistry::Enums> VariableRegistry::GetRegisteredEnums() const
{ return m_sEnums; }

bool VariableRegistry::try_GetID(Farg<std::string> inName, uint& outID) const
{
    if(auto found_it{mReferences.find(inName)}; found_it != mReferences.end())
    {
        outID = found_it->second;
        return true;
    }
    return false;
}

bool VariableRegistry::try_GetIDName(uint inID, std::string& outName) const
{
    for(FARG(auto) [name, id] : mReferences)
    {
        if(inID == id)
        {
            outName = name;
            return true;
        }
    }
    return false;
}

bool VariableRegistry::HasID(uint inID) const
{
    for(FAUTO [name, id] : mReferences)
    {
        if(inID == id)
            { return true; }
    }
    return false;
}

bool VariableRegistry::HasID(Sarg inName) const
{ return mReferences.contains(inName); }

uint VariableRegistry::GetID(Sarg inName) const
{
    uint out{};
    try_GetID(inName, out);
    return out;
}

std::string VariableRegistry::GetIDName(uint inID) const
{
    std::string out{};
    try_GetIDName(inID, out);
    return out;
}

Error VariableRegistry::RegisterID(Farg<std::string> inName, uint inID, bool noCopies)
{
    if(noCopies && mReferences.contains(inName))
        { print_error("[{}, {}]", inName, inID); return print_error_enum(ERR_ALREADY_EXISTS); }
    mReferences[inName] = inID;
    return OK;
}

Error VariableRegistry::RemoveID(Farg<std::string> inName)
{
    if(auto found_it{mReferences.find(inName)}; found_it != mReferences.end())
    {
        mReferences.erase(found_it);
        return OK;
    }
    return ERR_NOT_FOUND;
}

Error VariableRegistry::RemoveID(uint inID)
{
    for(FARG(auto) [name, id] : mReferences)
    {
        if(inID == id)
        {
            mReferences.erase(name);
            return OK;
        }
    }
    return ERR_NOT_FOUND;
}

void VariableRegistry::Init()
{
    PRINT_PRETTY_FUNCTION;
    RegisterEngineEnums();
    RegisterEngineReferences();
}

void VariableRegistry::ClearIDs()
{
    PRINT_PRETTY_FUNCTION;
    mReferences.clear();
    RegisterEngineReferences();
}

void VariableRegistry::ClearEnums()
{
    PRINT_PRETTY_FUNCTION;
    m_sEnums.clear();
    RegisterEngineEnums();
}

void VariableRegistry::RegisterEngineEnums()
{
    m_sEnums["Static"]         = MotionType::Static;
    m_sEnums["Dynamic"]        = MotionType::Dynamic;
    m_sEnums["Kinematic"]      = MotionType::Kinematic;
    m_sEnums["Box"]            = ShapeType::Box;
    m_sEnums["Capsule"]        = ShapeType::Capsule;
    m_sEnums["Cylinder"]       = ShapeType::Cylinder;
    m_sEnums["Sphere"]         = ShapeType::Sphere;
    m_sEnums["2DTexture"]      = TextureType::TEXTURE_TYPE_2D;
    m_sEnums["CubeMapTexture"] = TextureType::TEXTURE_TYPE_CUBE;
    m_sEnums["CustomColor"]    = Environment::BG_CUSTOM_COLOR;
    m_sEnums["ClearColor"]     = Environment::BG_CLEAR_COLOR;
    m_sEnums["Skybox"]         = Environment::BG_SKYBOX;
}

void VariableRegistry::RegisterEngineReferences()
{
    mReferences["ErrorModel"]      = UID::m_Error[];
    mReferences["DefaultCube"]     = UID::m_Cube[];
    mReferences["RamielModel"]     = UID::m_Ramiel[];
    mReferences["CameraModel"]     = UID::m_Camera3D[];
    mReferences["MissingTexture"]  = UID::t_Missing[];
    mReferences["LightTexture"]    = UID::t_LightDebug[];
    mReferences["DoomTexture"]     = UID::t_COMP04_5[];
    mReferences["LolBitTexture"]   = UID::t_LolBit[];
}
