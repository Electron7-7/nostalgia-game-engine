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
