#include "variable_registry.hpp"
#include "core/printing.hpp"
#include "core/uid.hpp"
#include "things/devices/collider.hpp"

static VariableRegistry sVariableRegistry{};
VariableRegistry* g_pVariableRegistry{&sVariableRegistry};

VariableRegistry::References VariableRegistry::m_sReferences{};
VariableRegistry::Enums VariableRegistry::m_sEnums{};

VariableRegistry::VariableRegistry() noexcept = default;

Farg<VariableRegistry::References> VariableRegistry::GetRegisteredIDs() const
{ return m_sReferences; }

Farg<VariableRegistry::Enums> VariableRegistry::GetRegisteredEnums() const
{ return m_sEnums; }

bool VariableRegistry::try_GetID(Farg<std::string> inName, uint& outID)
{
    if(auto found_it{m_sReferences.find(inName)}; found_it != m_sReferences.end())
    {
        outID = found_it->second;
        return true;
    }
    return false;
}

bool VariableRegistry::try_GetIDName(uint inID, std::string& outName)
{
    for(FARG(auto) [name, id] : m_sReferences)
    {
        if(inID == id)
        {
            outName = name;
            return true;
        }
    }
    return false;
}

Error VariableRegistry::RegisterID(Farg<std::string> inName, uint inID, bool noCopies)
{
    if(noCopies && m_sReferences.contains(inName))
        { return ERR_ALREADY_EXISTS; }
    m_sReferences[inName] = inID;
    return OK;
}

Error VariableRegistry::RemoveID(Farg<std::string> inName)
{
    if(auto found_it{m_sReferences.find(inName)}; found_it != m_sReferences.end())
    {
        m_sReferences.erase(found_it);
        return OK;
    }
    return ERR_NOT_FOUND;
}

Error VariableRegistry::RemoveID(uint inID)
{
    for(FARG(auto) [name, id] : m_sReferences)
    {
        if(inID == id)
        {
            m_sReferences.erase(name);
            return OK;
        }
    }
    return ERR_NOT_FOUND;
}

void VariableRegistry::Init()
{
    PRINT_PRETTY_FUNCTION;
    m_sReferences["MissingTexture"] = UID::i_Missing;
    m_sReferences["LightTexture"]   = UID::i_LightDebug;
    m_sReferences["DoomTexture"]    = UID::i_COMP04_5;
    m_sReferences["LolBitTexture"]  = UID::i_LolBit;
    m_sReferences["ErrorModel"]     = UID::m_Error;
    m_sReferences["DefaultCube"]    = UID::m_Cube;
    m_sReferences["RamielModel"]    = UID::m_Ramiel;
    m_sReferences["Audiowide"]      = UID::f_Audiowide;
    m_sReferences["DejaVuSans"]     = UID::f_DejaVuSans;
    m_sReferences["Verdana"]        = UID::f_Verdana;
    m_sEnums["Static"]              = PhysicsBodyMotion::Static;
    m_sEnums["Dynamic"]             = PhysicsBodyMotion::Dynamic;
    m_sEnums["Kinematic"]           = PhysicsBodyMotion::Kinematic;
    m_sEnums["Box"]                 = PhysicsBodyShape::Box;
    m_sEnums["Capsule"]             = PhysicsBodyShape::Capsule;
    m_sEnums["Cylinder"]            = PhysicsBodyShape::Cylinder;
    m_sEnums["Sphere"]              = PhysicsBodyShape::Sphere;
}

void VariableRegistry::ClearIDs()
{
    PRINT_PRETTY_FUNCTION;
    m_sReferences.clear();
    Init();
}

void VariableRegistry::ClearEnums()
{
    PRINT_PRETTY_FUNCTION;
    m_sEnums.clear();
    Init();
}
