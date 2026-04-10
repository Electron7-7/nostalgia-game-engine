#include "./resource_manager.hpp"
#include "models.hpp"
#include "images.hpp"
#include "fonts.hpp"
#include "things/thing_factory.hpp"
#include "things/resource_database.hpp"
#include "things/resources/cubemap.hpp"
#include "things/resources/image.hpp"
#include "things/resources/array_mesh.hpp"
#include "things/resources/font.hpp"

ResourceManager sResourceManager{};
ResourceManager* g_pResourceManager{&sResourceManager};

bool ResourceManager::Init()
{
    PRINT_PRETTY_FUNCTION;

    if(not ThingFactory::Init())
        { return print_error("ThingFactory::Init failed"); }

    EnumRegistry::Register(Mesh::ARRAY_FORMAT_VERTEX,  "Position");
    EnumRegistry::Register(Mesh::ARRAY_FORMAT_COLORS,  "Color");
    EnumRegistry::Register(Mesh::ARRAY_FORMAT_NORMALS, "Normal");
    EnumRegistry::Register(Mesh::ARRAY_FORMAT_UV,      "UV");
    EnumRegistry::Register(Mesh::PRIMITIVE_TRIANGLES,  "Triangles");
    EnumRegistry::Register(Mesh::PRIMITIVE_POINTS,     "Points");
    EnumRegistry::Register(Mesh::PRIMITIVE_LINES,      "Lines");

    UID::f_Audiowide =
        ResourceDatabase::Register(Font::CreateFromMemory(Fonts::Audiowide,
            std::size(Fonts::Audiowide)), "Audiowide");
    UID::f_DejaVuSans =
        ResourceDatabase::Register(Font::CreateFromMemory(Fonts::DejaVuSans,
            std::size(Fonts::DejaVuSans)), "DejaVuSans");
    UID::f_Verdana =
        ResourceDatabase::Register(Font::CreateFromMemory(Fonts::Verdana,
            std::size(Fonts::Verdana)), "Verdana");
    UID::m_Error =
        ResourceDatabase::Register(ArrayMesh::CreateFromMemory(Models::Error,
            std::size(Models::Error), ArrayMesh::MODEL_OBJ), "ErrorModel");
    UID::m_Cube =
        ResourceDatabase::Register(ArrayMesh::CreateFromMemory(Models::Cube,
            std::size(Models::Cube), ArrayMesh::MODEL_OBJ), "DefaultCube");
    UID::m_Quad =
        ResourceDatabase::Register(ArrayMesh::CreateFromMemory(Models::Quad,
            std::size(Models::Quad), ArrayMesh::MODEL_OBJ), "DefaultQuad");
    UID::m_Ramiel =
        ResourceDatabase::Register(ArrayMesh::CreateFromMemory(Models::Ramiel,
            std::size(Models::Ramiel), ArrayMesh::MODEL_OBJ), "RamielModel");
    UID::m_Camera3D =
        ResourceDatabase::Register(ArrayMesh::CreateFromMemory(Models::Camera,
            std::size(Models::Camera), ArrayMesh::MODEL_OBJ), "CameraModel");
    UID::m_DebugAxis =
        ResourceDatabase::Register(ArrayMesh::CreateFromMemory(Models::DebugAxis,
            std::size(Models::DebugAxis), ArrayMesh::MODEL_OBJ), "3DAxisModel");
    UID::i_Missing =
        ResourceDatabase::Register(Image::CreateFromData(Images::Missing,
            std::size(Images::Missing)), "MissingImage");
    UID::i_LightDebug =
        ResourceDatabase::Register(Image::CreateFromData(Images::LightDebug,
            std::size(Images::LightDebug)), "LightImage");
    UID::i_COMP04_5 =
        ResourceDatabase::Register(Image::CreateFromData(Images::COMP04_5,
            std::size(Images::COMP04_5)), "DoomImage");
    UID::i_LolBit =
        ResourceDatabase::Register(Image::CreateFromData(Images::LolBit,
            std::size(Images::LolBit)), "LolBitImage");
    UID::t_ShittySkybox =
        ResourceDatabase::Register(Cubemap::CreateFromImages({
            Image::CreateFromData(Images::SkyboxXp, std::size(Images::SkyboxXp)),
            Image::CreateFromData(Images::SkyboxXn, std::size(Images::SkyboxXn)),
            Image::CreateFromData(Images::SkyboxYp, std::size(Images::SkyboxYp)),
            Image::CreateFromData(Images::SkyboxYn, std::size(Images::SkyboxYn)),
            Image::CreateFromData(Images::SkyboxZp, std::size(Images::SkyboxZp)),
            Image::CreateFromData(Images::SkyboxZn, std::size(Images::SkyboxZn))}),
        "ShittySkybox");

    return true;
}
