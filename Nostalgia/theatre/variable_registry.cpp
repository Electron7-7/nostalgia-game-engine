#include "rendering/texture_buffer.hpp"
#include "things/thinkers/3d/collider_3d.hpp"
#include "rendering/environment.hpp"
#include "models.hpp"
#include "images.hpp"
#include "fonts.hpp"

static auto s_pAudiowide  {MakeShared<FileData>(Fonts::Audiowide,   std::size(Fonts::Audiowide),   FileType::font_TTF )};
static auto s_pVerdana    {MakeShared<FileData>(Fonts::Verdana,     std::size(Fonts::Verdana),     FileType::font_TTF )};
static auto s_pDejaVuSans {MakeShared<FileData>(Fonts::DejaVuSans,  std::size(Fonts::DejaVuSans),  FileType::font_TTF )};
static auto s_pError      {MakeShared<FileData>(Models::Error,      std::size(Models::Error),      FileType::model_OBJ)};
static auto s_pCube       {MakeShared<FileData>(Models::Cube,       std::size(Models::Cube),       FileType::model_OBJ)};
static auto s_pQuad       {MakeShared<FileData>(Models::Quad,       std::size(Models::Quad),       FileType::model_OBJ)};
static auto s_pRamiel     {MakeShared<FileData>(Models::Ramiel,     std::size(Models::Ramiel),     FileType::model_OBJ)};
static auto s_pCamera     {MakeShared<FileData>(Models::Camera,     std::size(Models::Camera),     FileType::model_OBJ)};
static auto s_pDebugAxis  {MakeShared<FileData>(Models::DebugAxis,  std::size(Models::DebugAxis),  FileType::model_OBJ)};
static auto s_pMissing    {MakeShared<FileData>(Images::Missing,    std::size(Images::Missing),    FileType::image_PNG)};
static auto s_pLolBit     {MakeShared<FileData>(Images::LolBit,     std::size(Images::LolBit),     FileType::image_PNG)};
static auto s_pLightDebug {MakeShared<FileData>(Images::LightDebug, std::size(Images::LightDebug), FileType::image_JPG)};
static auto s_pCOMP04_5   {MakeShared<FileData>(Images::COMP04_5,   std::size(Images::COMP04_5),   FileType::image_PNG)};
static auto s_pSkyboxXn   {MakeShared<FileData>(Images::SkyboxXn,   std::size(Images::SkyboxXn),   FileType::image_PNG)};
static auto s_pSkyboxXp   {MakeShared<FileData>(Images::SkyboxXp,   std::size(Images::SkyboxXp),   FileType::image_PNG)};
static auto s_pSkyboxYp   {MakeShared<FileData>(Images::SkyboxYp,   std::size(Images::SkyboxYp),   FileType::image_PNG)};
static auto s_pSkyboxYn   {MakeShared<FileData>(Images::SkyboxYn,   std::size(Images::SkyboxYn),   FileType::image_PNG)};
static auto s_pSkyboxZn   {MakeShared<FileData>(Images::SkyboxZn,   std::size(Images::SkyboxZn),   FileType::image_PNG)};
static auto s_pSkyboxZp   {MakeShared<FileData>(Images::SkyboxZp,   std::size(Images::SkyboxZp),   FileType::image_PNG)};

RMutex VariableRegistry::m_sEnumsMutex{},
    VariableRegistry::m_sResourceDataMutex{};
VariableRegistry::Enums VariableRegistry::m_sEnums{};
VariableRegistry::ResourceData VariableRegistry::m_sResourceData{};

VariableRegistry::VariableRegistry() noexcept = default;

Farg<VariableRegistry::Enums> VariableRegistry::GetRegisteredEnums()
{ return m_sEnums; }

Farg<VariableRegistry::ResourceData> VariableRegistry::GetRegisteredResourceData()
{ return m_sResourceData; }

void VariableRegistry::ClearEnums()
{
    LockGuard<RMutex> enums_lock{m_sEnumsMutex};
    PRINT_PRETTY_FUNCTION;
    m_sEnums.clear();
    RegisterEngineEnums();
}

bool VariableRegistry::try_GetResourceData(Sarg inName, Shared<FileData>& outData)
{
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    for(FAUTO [pid, data] : m_sResourceData)
        { if(pid == inName) { outData = data; return true; } }
    return false;
}

bool VariableRegistry::try_GetResourceData(ID inID, Shared<FileData>& outData)
{
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    for(FAUTO [pid, data] : m_sResourceData)
        { if(pid == inID) { outData = data; return true; } }
    return false;
}

Shared<FileData> VariableRegistry::GetResourceData(Sarg inName)
{
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    for(FAUTO [pid, data] : m_sResourceData)
        { if(pid == inName) { return data; } }
    return MakeShared<FileData>();
}

Shared<FileData> VariableRegistry::GetResourceData(ID inID)
{
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    for(FAUTO [pid, data] : m_sResourceData)
        { if(pid == inID) { return data; } }
    return MakeShared<FileData>();
}

bool VariableRegistry::HasResourceData(Sarg inName)
{
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    for(FAUTO [pid, data] : m_sResourceData)
        { if(pid == inName) { return true; } }
    return false;
}

bool VariableRegistry::HasResourceData(ID inID)
{
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    for(FAUTO [pid, data] : m_sResourceData)
        { if(pid == inID) { return true; } }
    return false;
}

Error VariableRegistry::RegisterResourceData(ID inID,
    UID::ReservedType inType,
    Sarg inName,
    Farg<Shared<FileData>> inData,
    bool doNoCopies)
{
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    if(doNoCopies)
    {
        for(FAUTO [pid, data] : m_sResourceData)
        {
            if(not pid.name().compare(inName) or data->Data() == inData->Data())
                { return ERR_ALREADY_EXISTS; }
        }
    }
    if(auto status{UID::CreateReservedUID(inType, inID)}; not status)
        { return status; }
    m_sResourceData[{inID(), inName}] = inData;
    return OK;
}

Error VariableRegistry::RemoveResourceData(Sarg inName)
{
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    for(FAUTO [pid, data] : m_sResourceData)
    {
        if(pid == inName)
        {
            m_sResourceData.erase(pid);
            return UID::EraseReservedUID(pid.id());
        }
    }
    return ERR_NOT_FOUND;
}

Error VariableRegistry::RemoveResourceData(ID inID)
{
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    for(FAUTO [pid, data] : m_sResourceData)
    {
        if(pid == inID())
        {
            m_sResourceData.erase(pid);
            return UID::EraseReservedUID(pid.id());
        }
    }
    return ERR_NOT_FOUND;
}

void VariableRegistry::ClearResourceData()
{
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    for(auto pair{m_sResourceData.begin()}; pair != m_sResourceData.end();)
    {
        if(UID::EraseReservedUID(pair->first.id()) == OK)
        {
            pair = m_sResourceData.erase(pair);
            continue;
        }
        ++pair;
    }
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

void VariableRegistry::RegisterEngineResourceData()
{
    m_sResourceData[{ UID::f_Audiowide(),      "Audiowide"      }] = s_pAudiowide;
    m_sResourceData[{ UID::f_Verdana(),        "Verdana"        }] = s_pVerdana;
    m_sResourceData[{ UID::f_DejaVuSans(),     "DejaVuSans"     }] = s_pDejaVuSans;
    m_sResourceData[{ UID::m_Error(),          "ErrorModel"     }] = s_pError;
    m_sResourceData[{ UID::m_Cube(),           "DefaultCube"    }] = s_pCube;
    m_sResourceData[{ UID::m_Quad(),           "DefaultQuad"    }] = s_pQuad;
    m_sResourceData[{ UID::m_Ramiel(),         "RamielModel"    }] = s_pRamiel;
    m_sResourceData[{ UID::m_Camera3D(),       "CameraModel"    }] = s_pCamera;
    m_sResourceData[{ UID::m_DebugAxis(),      "DebugAxis"      }] = s_pDebugAxis;
    m_sResourceData[{ UID::i_Missing(),        "MissingTexture" }] = s_pMissing;
    m_sResourceData[{ UID::i_LolBit(),         "LolBitTexture"  }] = s_pLolBit;
    m_sResourceData[{ UID::i_LightDebug(),     "LightTexture"   }] = s_pLightDebug;
    m_sResourceData[{ UID::i_COMP04_5(),       "DoomTexture"    }] = s_pCOMP04_5;
    m_sResourceData[{ UID::i_ShittySkyboxXn(), "ShittySkybox01" }] = s_pSkyboxXn;
    m_sResourceData[{ UID::i_ShittySkyboxXp(), "ShittySkybox02" }] = s_pSkyboxXp;
    m_sResourceData[{ UID::i_ShittySkyboxYp(), "ShittySkybox03" }] = s_pSkyboxYp;
    m_sResourceData[{ UID::i_ShittySkyboxYn(), "ShittySkybox04" }] = s_pSkyboxYn;
    m_sResourceData[{ UID::i_ShittySkyboxZn(), "ShittySkybox05" }] = s_pSkyboxZn;
    m_sResourceData[{ UID::i_ShittySkyboxZp(), "ShittySkybox06" }] = s_pSkyboxZp;
}
