#include "rendering/texture_buffer.hpp"
#include "things/thinkers/3d/collider_3d.hpp"
#include "rendering/environment.hpp"
#include "models.hpp"
#include "images.hpp"
#include "fonts.hpp"

#define FOUND_IT(NAME) auto found_it{m_sResourceData.find(NAME)}; found_it != m_sResourceData.end()

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

bool VariableRegistry::try_GetResourceData(Sarg inName, Shared<FileData>& outResource)
{
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    if(FOUND_IT(inName))
    {
        outResource = found_it->second;
        return true;
    }
    return false;
}

bool VariableRegistry::try_GetResourceDataName(Farg<Shared<FileData>> inResourceData, std::string& outName)
{
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    if(FAUTO name{GetResourceDataName(inResourceData)}; not name.empty())
    {
        outName = name;
        return true;
    }
    return false;
}

Shared<FileData> VariableRegistry::GetResourceData(Sarg inName)
{
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    if(FOUND_IT(inName))
        { return found_it->second; }
    return MakeShared<FileData>();
}

Sarg VariableRegistry::GetResourceDataName(Farg<Shared<FileData>> inResourceData)
{
    static std::string empty{};
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    for(FAUTO [name, data] : m_sResourceData)
    {
        if(inResourceData->Data() == data->Data())
            { return name; }
    }
    return empty;
}

bool VariableRegistry::HasResourceData(Sarg inName)
{
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    for(FAUTO [pid, data] : m_sResourceData)
        { if(pid == inName) { return true; } }
    return false;
}

Error VariableRegistry::RegisterResourceData(Sarg inName, Farg<Shared<FileData>> inResource, bool doNoCopies)
{
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    if(doNoCopies and m_sResourceData.contains(inName))
        { return ERR_ALREADY_EXISTS; }
    m_sResourceData[inName] = inResource;
    return OK;
}

Error VariableRegistry::RemoveResourceData(Sarg inName)
{
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    return (m_sResourceData.erase(inName))
        ? OK
        : ERR_NOT_FOUND;
}

void VariableRegistry::ClearResourceData()
{
    LockGuard<RMutex> resource_data_lock{m_sResourceDataMutex};
    m_sResourceData.clear();
    RegisterEngineResourceData();
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
    m_sResourceData[{ "Audiowide"      }] = MakeShared<FileData>(Fonts::Audiowide,   std::size(Fonts::Audiowide),   FileType::font_TTF );
    m_sResourceData[{ "Verdana"        }] = MakeShared<FileData>(Fonts::Verdana,     std::size(Fonts::Verdana),     FileType::font_TTF );
    m_sResourceData[{ "DejaVuSans"     }] = MakeShared<FileData>(Fonts::DejaVuSans,  std::size(Fonts::DejaVuSans),  FileType::font_TTF );
    m_sResourceData[{ "ErrorModel"     }] = MakeShared<FileData>(Models::Error,      std::size(Models::Error),      FileType::model_OBJ);
    m_sResourceData[{ "DefaultCube"    }] = MakeShared<FileData>(Models::Cube,       std::size(Models::Cube),       FileType::model_OBJ);
    m_sResourceData[{ "DefaultQuad"    }] = MakeShared<FileData>(Models::Quad,       std::size(Models::Quad),       FileType::model_OBJ);
    m_sResourceData[{ "RamielModel"    }] = MakeShared<FileData>(Models::Ramiel,     std::size(Models::Ramiel),     FileType::model_OBJ);
    m_sResourceData[{ "CameraModel"    }] = MakeShared<FileData>(Models::Camera,     std::size(Models::Camera),     FileType::model_OBJ);
    m_sResourceData[{ "DebugAxis"      }] = MakeShared<FileData>(Models::DebugAxis,  std::size(Models::DebugAxis),  FileType::model_OBJ);
    m_sResourceData[{ "MissingTexture" }] = MakeShared<FileData>(Images::Missing,    std::size(Images::Missing),    FileType::image_PNG);
    m_sResourceData[{ "LolBitTexture"  }] = MakeShared<FileData>(Images::LolBit,     std::size(Images::LolBit),     FileType::image_PNG);
    m_sResourceData[{ "LightTexture"   }] = MakeShared<FileData>(Images::LightDebug, std::size(Images::LightDebug), FileType::image_JPG);
    m_sResourceData[{ "DoomTexture"    }] = MakeShared<FileData>(Images::COMP04_5,   std::size(Images::COMP04_5),   FileType::image_PNG);
    m_sResourceData[{ "ShittySkybox01" }] = MakeShared<FileData>(Images::SkyboxXp,   std::size(Images::SkyboxXp),   FileType::image_PNG);
    m_sResourceData[{ "ShittySkybox02" }] = MakeShared<FileData>(Images::SkyboxXn,   std::size(Images::SkyboxXn),   FileType::image_PNG);
    m_sResourceData[{ "ShittySkybox03" }] = MakeShared<FileData>(Images::SkyboxYp,   std::size(Images::SkyboxYp),   FileType::image_PNG);
    m_sResourceData[{ "ShittySkybox04" }] = MakeShared<FileData>(Images::SkyboxYn,   std::size(Images::SkyboxYn),   FileType::image_PNG);
    m_sResourceData[{ "ShittySkybox05" }] = MakeShared<FileData>(Images::SkyboxZp,   std::size(Images::SkyboxZp),   FileType::image_PNG);
    m_sResourceData[{ "ShittySkybox06" }] = MakeShared<FileData>(Images::SkyboxZn,   std::size(Images::SkyboxZn),   FileType::image_PNG);
}
