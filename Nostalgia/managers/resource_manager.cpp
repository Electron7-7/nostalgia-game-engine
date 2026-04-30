#include "./resource_manager.hpp"
#include "models.hpp"
#include "images.hpp"
#include "fonts.hpp"
#include "rendering/environment.hpp"
#include "things/thing_factory.hpp"
#include "things/resource_database.hpp"
#include "things/resources/cubemap.hpp"
#include "things/resources/image_texture.hpp"
#include "things/resources/array_mesh.hpp"
#include "things/resources/font.hpp"

ResourceManager sResourceManager{};
ResourceManager* g_pResourceManager{&sResourceManager};

bool ResourceManager::Init()
{
    PRINT_PRETTY_FUNCTION;

    if(not ThingFactory::Init())
        { return print_error("ThingFactory::Init failed"); }

    EnumRegistry::Register(TEXTURE_TYPE_1D,       "TEXTURE_TYPE_1D");
    EnumRegistry::Register(TEXTURE_TYPE_2D,       "TEXTURE_TYPE_2D");
    EnumRegistry::Register(TEXTURE_TYPE_3D,       "TEXTURE_TYPE_3D");
    EnumRegistry::Register(TEXTURE_TYPE_1D_ARRAY, "TEXTURE_TYPE_1D_ARRAY");
    EnumRegistry::Register(TEXTURE_TYPE_2D_ARRAY, "TEXTURE_TYPE_2D_ARRAY");
    EnumRegistry::Register(TEXTURE_TYPE_CUBE,     "TEXTURE_TYPE_CUBE");

    EnumRegistry::Register(SAMPLER_FILTER_NEAREST, "SamplerFilterNearest");
    EnumRegistry::Register(SAMPLER_FILTER_LINEAR,  "SamplerFilterLinear");
    EnumRegistry::Register(SAMPLER_FILTER_NONE,    "SamplerNoFilter");

    EnumRegistry::Register(SAMPLER_REPEAT_MODE_REPEAT,               "SamplerRepeat");
    EnumRegistry::Register(SAMPLER_REPEAT_MODE_MIRRORED_REPEAT,      "SamplerRepeatMirrored");
    EnumRegistry::Register(SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE,        "SamplerClampEdge");
    EnumRegistry::Register(SAMPLER_REPEAT_MODE_CLAMP_TO_BORDER,      "SamplerClampBorder");
    EnumRegistry::Register(SAMPLER_REPEAT_MODE_MIRROR_CLAMP_TO_EDGE, "SamplerMirrorClampEdge");

    EnumRegistry::Register(Mesh::ARRAY_FORMAT_VERTEX,  "Position");
    EnumRegistry::Register(Mesh::ARRAY_FORMAT_COLORS,  "Color");
    EnumRegistry::Register(Mesh::ARRAY_FORMAT_NORMALS, "Normal");
    EnumRegistry::Register(Mesh::ARRAY_FORMAT_UV,      "UV");
    EnumRegistry::Register(Mesh::PRIMITIVE_TRIANGLES,  "Triangles");
    EnumRegistry::Register(Mesh::PRIMITIVE_POINTS,     "Points");
    EnumRegistry::Register(Mesh::PRIMITIVE_LINES,      "Lines");

    EnumRegistry::Assign(Environment::BG_CUSTOM_COLOR, "CustomColor");
    EnumRegistry::Assign(Environment::BG_CLEAR_COLOR,  "ClearColor");
    EnumRegistry::Assign(Environment::BG_SKYBOX,       "Skybox");

    EnumRegistry::Register(IBuffer::Element::Type::None,   "IBuffer::Element::Type::None");
    EnumRegistry::Register(IBuffer::Element::Type::Float,  "Float");
    EnumRegistry::Register(IBuffer::Element::Type::Float2, "Float2");
    EnumRegistry::Register(IBuffer::Element::Type::Float3, "Float3");
    EnumRegistry::Register(IBuffer::Element::Type::Float4, "Float4");
    EnumRegistry::Register(IBuffer::Element::Type::Int,    "Int");
    EnumRegistry::Register(IBuffer::Element::Type::Int2,   "Int2");
    EnumRegistry::Register(IBuffer::Element::Type::Int3,   "Int3");
    EnumRegistry::Register(IBuffer::Element::Type::Int4,   "Int4");
    EnumRegistry::Register(IBuffer::Element::Type::Mat3,   "Mat3");
    EnumRegistry::Register(IBuffer::Element::Type::Mat4,   "Mat4");
    EnumRegistry::Register(IBuffer::Element::Type::Bool,   "Bool");

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
    UID::t_Missing =
        ResourceDatabase::Register(ImageTexture::CreateFromImage(Image::CreateFromFile(
            {Images::Missing, std::size(Images::Missing)})), "MissingImage");
    UID::t_LightDebug =
        ResourceDatabase::Register(ImageTexture::CreateFromImage(Image::CreateFromFile(
            {Images::LightDebug, std::size(Images::LightDebug)})), "LightImage");
    UID::t_COMP04_5 =
        ResourceDatabase::Register(ImageTexture::CreateFromImage(Image::CreateFromFile(
            {Images::COMP04_5, std::size(Images::COMP04_5)})), "DoomImage");
    UID::t_LolBit =
        ResourceDatabase::Register(ImageTexture::CreateFromImage(Image::CreateFromFile(
            {Images::LolBit, std::size(Images::LolBit)})), "LolBitImage");
    UID::t_ShittySkybox =
        ResourceDatabase::Register(Cubemap::CreateFromImages({
            Image::CreateFromFile({Images::SkyboxXp, std::size(Images::SkyboxXp)}),
            Image::CreateFromFile({Images::SkyboxXn, std::size(Images::SkyboxXn)}),
            Image::CreateFromFile({Images::SkyboxYp, std::size(Images::SkyboxYp)}),
            Image::CreateFromFile({Images::SkyboxYn, std::size(Images::SkyboxYn)}),
            Image::CreateFromFile({Images::SkyboxZp, std::size(Images::SkyboxZp)}),
            Image::CreateFromFile({Images::SkyboxZn, std::size(Images::SkyboxZn)})}),
        "ShittySkybox");

    return true;
}
