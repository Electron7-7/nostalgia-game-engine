#include "./resource_manager.hpp"
#include "models.hpp"
#include "images.hpp"
#include "fonts.hpp"
#include "core/variant.hpp"
#include "things/thing_factory.hpp"
#include "things/resources/cubemap.hpp"
#include "things/resources/image_texture.hpp"
#include "things/resources/array_mesh.hpp"
#include "things/resources/font.hpp"
#include "rendering/environment.hpp"

ResourceManager sResourceManager{};
ResourceManager* g_pResourceManager{&sResourceManager};

bool ResourceManager::Init()
{
    PRINT_PRETTY_FUNCTION;

    if(not ThingFactory::Init())
        { return print_error("ThingFactory::Init failed"); }

    EnumRegistry::Register(Variant::NIL,        "Variant::Nil");
    EnumRegistry::Register(Variant::BOOL,       "Variant::Bool");
    EnumRegistry::Register(Variant::INT,        "Variant::Int");
    EnumRegistry::Register(Variant::FLOAT,      "Variant::Float");
    EnumRegistry::Register(Variant::STRING,     "Variant::String");
    EnumRegistry::Register(Variant::VECTOR2,    "Variant::Vector2");
    EnumRegistry::Register(Variant::VECTOR3,    "Variant::Vector3");
    EnumRegistry::Register(Variant::VECTOR4,    "Variant::Vector4");
    EnumRegistry::Register(Variant::QUATERNION, "Variant::Quaternion");
    EnumRegistry::Register(Variant::THING,      "Variant::Thing");
    EnumRegistry::Register(Variant::TYPE_MAX,   "Variant::Invalid");

    EnumRegistry::Register(TEXTURE_TYPE_1D,       "Texture::1D");
    EnumRegistry::Register(TEXTURE_TYPE_2D,       "Texture::2D");
    EnumRegistry::Register(TEXTURE_TYPE_3D,       "Texture::3D");
    EnumRegistry::Register(TEXTURE_TYPE_1D_ARRAY, "Texture::1DArray");
    EnumRegistry::Register(TEXTURE_TYPE_2D_ARRAY, "Texture::2DArray");
    EnumRegistry::Register(TEXTURE_TYPE_CUBE,     "Texture::Cubemap");

    EnumRegistry::Register(SAMPLER_FILTER_NEAREST, "Sampler::FilterNearest");
    EnumRegistry::Register(SAMPLER_FILTER_LINEAR,  "Sampler::FilterLinear");
    EnumRegistry::Register(SAMPLER_FILTER_NONE,    "Sampler::NoFilter");

    EnumRegistry::Register(SAMPLER_REPEAT_MODE_REPEAT,               "Sampler::Repeat");
    EnumRegistry::Register(SAMPLER_REPEAT_MODE_MIRRORED_REPEAT,      "Sampler::RepeatMirrored");
    EnumRegistry::Register(SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE,        "Sampler::ClampEdge");
    EnumRegistry::Register(SAMPLER_REPEAT_MODE_CLAMP_TO_BORDER,      "Sampler::ClampBorder");
    EnumRegistry::Register(SAMPLER_REPEAT_MODE_MIRROR_CLAMP_TO_EDGE, "Sampler::MirrorClampEdge");

    EnumRegistry::Register(Mesh::ARRAY_FORMAT_VERTEX,  "Vertex::Position");
    EnumRegistry::Register(Mesh::ARRAY_FORMAT_COLORS,  "Vertex::Color");
    EnumRegistry::Register(Mesh::ARRAY_FORMAT_NORMALS, "Vertex::Normal");
    EnumRegistry::Register(Mesh::ARRAY_FORMAT_UV,      "Vertex::UV");
    EnumRegistry::Register(Mesh::PRIMITIVE_TRIANGLES,  "Primitive::Triangles");
    EnumRegistry::Register(Mesh::PRIMITIVE_POINTS,     "Primitive::Points");
    EnumRegistry::Register(Mesh::PRIMITIVE_LINES,      "Primitive::Lines");

    EnumRegistry::Register(Environment::BG_CUSTOM_COLOR, "Environment::CustomColor");
    EnumRegistry::Register(Environment::BG_CLEAR_COLOR,  "Environment::ClearColor");
    EnumRegistry::Register(Environment::BG_SKYBOX,       "Environment::Skybox");

    EnumRegistry::Register(IBuffer::Element::Type::None,   "Element::None");
    EnumRegistry::Register(IBuffer::Element::Type::Float,  "Element::Float");
    EnumRegistry::Register(IBuffer::Element::Type::Float2, "Element::Float2");
    EnumRegistry::Register(IBuffer::Element::Type::Float3, "Element::Float3");
    EnumRegistry::Register(IBuffer::Element::Type::Float4, "Element::Float4");
    EnumRegistry::Register(IBuffer::Element::Type::Int,    "Element::Int");
    EnumRegistry::Register(IBuffer::Element::Type::Int2,   "Element::Int2");
    EnumRegistry::Register(IBuffer::Element::Type::Int3,   "Element::Int3");
    EnumRegistry::Register(IBuffer::Element::Type::Int4,   "Element::Int4");
    EnumRegistry::Register(IBuffer::Element::Type::Mat3,   "Element::Mat3");
    EnumRegistry::Register(IBuffer::Element::Type::Mat4,   "Element::Mat4");
    EnumRegistry::Register(IBuffer::Element::Type::Bool,   "Element::Bool");

    auto _Audiowide{Font::CreateFromMemory(Fonts::Audiowide, std::size(Fonts::Audiowide))};
    auto _DejaVuSans{Font::CreateFromMemory(Fonts::DejaVuSans, std::size(Fonts::DejaVuSans))};
    auto _Verdana{Font::CreateFromMemory(Fonts::Verdana, std::size(Fonts::Verdana))};
    auto _Error{ArrayMesh::CreateFromMemory(Models::Error, std::size(Models::Error), ArrayMesh::MODEL_OBJ)};
    auto _Cube{ArrayMesh::CreateFromMemory(Models::Cube, std::size(Models::Cube), ArrayMesh::MODEL_OBJ)};
    auto _Quad{ArrayMesh::CreateFromMemory(Models::Quad, std::size(Models::Quad), ArrayMesh::MODEL_OBJ)};
    auto _Ramiel{ArrayMesh::CreateFromMemory(Models::Ramiel, std::size(Models::Ramiel), ArrayMesh::MODEL_OBJ)};
    auto _Missing{ImageTexture::CreateFromImage(Image::CreateFromFile(
        {Images::Missing, std::size(Images::Missing)}))};
    auto _COMP04_5{ImageTexture::CreateFromImage(Image::CreateFromFile(
        {Images::COMP04_5, std::size(Images::COMP04_5)}))};
    auto _LolBit{ImageTexture::CreateFromImage(Image::CreateFromFile(
        {Images::LolBit, std::size(Images::LolBit)}))};
    auto _ShittySkybox{Cubemap::CreateFromImages({
        Image::CreateFromFile({Images::SkyboxXp, std::size(Images::SkyboxXp)}),
        Image::CreateFromFile({Images::SkyboxXn, std::size(Images::SkyboxXn)}),
        Image::CreateFromFile({Images::SkyboxYp, std::size(Images::SkyboxYp)}),
        Image::CreateFromFile({Images::SkyboxYn, std::size(Images::SkyboxYn)}),
        Image::CreateFromFile({Images::SkyboxZp, std::size(Images::SkyboxZp)}),
        Image::CreateFromFile({Images::SkyboxZn, std::size(Images::SkyboxZn)})})};

    UID::f_Audiowide    = _Audiowide->uid();
    UID::f_DejaVuSans   = _DejaVuSans->uid();
    UID::f_Verdana      = _Verdana->uid();
    UID::m_Error        = _Error->uid();
    UID::m_Cube         = _Cube->uid();
    UID::m_Quad         = _Quad->uid();
    UID::m_Ramiel       = _Ramiel->uid();
    UID::t_Missing      = _Missing->uid();
    UID::t_COMP04_5     = _COMP04_5->uid();
    UID::t_LolBit       = _LolBit->uid();
    UID::t_ShittySkybox = _ShittySkybox->uid();

    _Audiowide->rename("Audiowide");
    _DejaVuSans->rename("DejaVuSans");
    _Verdana->rename("Verdana");
    _Error->rename("ErrorModel");
    _Cube->rename("DefaultCube");
    _Quad->rename("DefaultQuad");
    _Ramiel->rename("RamielModel");
    _Missing->rename("MissingTexture");
    _COMP04_5->rename("DoomTexture");
    _LolBit->rename("LolBitTexture");
    _ShittySkybox->rename("ShittySkybox");

    return true;
}

void ResourceManager::Shutdown()
{ ThingFactory::Shutdown(); }
