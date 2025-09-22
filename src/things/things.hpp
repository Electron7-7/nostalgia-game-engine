#ifndef THINGS_H
#define THINGS_H

#include "hash.hpp"

#include <memory>

namespace TypeName
{
    constexpr const char* Invalid          = "Invalid";
    constexpr const char* Thing            = "Thing";
    constexpr const char* Resource         = "Resource";
    constexpr const char* Mesh             = "Mesh";
    constexpr const char* Font             = "Font";
    constexpr const char* Texture          = "Texture";
    constexpr const char* Material         = "Material";
    constexpr const char* MeshInstance     = "MeshInstance";
    constexpr const char* Thinker          = "Thinker";
    constexpr const char* Actor            = "Actor";
    constexpr const char* PointLight       = "PointLight";
    constexpr const char* SpotLight        = "SpotLight";
    constexpr const char* DirectionalLight = "DirectionalLight";
    constexpr const char* NostalgiaPlayer  = "NostalgiaPlayer";
}

namespace Type
{
    constexpr size_t Invalid          = ConstexprHash(TypeName::Invalid);
    constexpr size_t Thing            = ConstexprHash(TypeName::Thing);
    constexpr size_t Resource         = ConstexprHash(TypeName::Resource);
    constexpr size_t Mesh             = ConstexprHash(TypeName::Mesh);
    constexpr size_t Font             = ConstexprHash(TypeName::Font);
    constexpr size_t Texture          = ConstexprHash(TypeName::Texture);
    constexpr size_t Material         = ConstexprHash(TypeName::Material);
    constexpr size_t MeshInstance     = ConstexprHash(TypeName::MeshInstance);
    constexpr size_t Thinker          = ConstexprHash(TypeName::Thinker);
    constexpr size_t Actor            = ConstexprHash(TypeName::Actor);
    constexpr size_t PointLight       = ConstexprHash(TypeName::PointLight);
    constexpr size_t SpotLight        = ConstexprHash(TypeName::SpotLight);
    constexpr size_t DirectionalLight = ConstexprHash(TypeName::DirectionalLight);
    constexpr size_t NostalgiaPlayer  = ConstexprHash(TypeName::NostalgiaPlayer);
}

class Thing; // Forward Declaration

std::shared_ptr<Thing>(*g_MakeThing(size_t TypeID))();
template<typename T>
std::shared_ptr<T> g_GetThing(id_t ID);

const int  g_GetPriority(size_t TypeID);
const bool g_IsValidType(const std::string& TypeName);
const bool g_IsValidType(size_t TypeID);

#ifdef DEBUGGING
inline const char* const& StringifyType(size_t TypeID)
{
    switch(TypeID)
    {
    case Type::Invalid:
        return TypeName::Invalid;
    case Type::Resource:
        return TypeName::Resource;
    case Type::Mesh:
        return TypeName::Mesh;
    case Type::Texture:
        return TypeName::Texture;
    case Type::Material:
        return TypeName::Material;
    case Type::MeshInstance:
        return TypeName::MeshInstance;
    case Type::Thing:
        return TypeName::Thing;
    case Type::Actor:
        return TypeName::Actor;
    case Type::PointLight:
        return TypeName::PointLight;
    case Type::SpotLight:
        return TypeName::SpotLight;
    case Type::DirectionalLight:
        return TypeName::DirectionalLight;
    case Type::NostalgiaPlayer:
        return TypeName::NostalgiaPlayer;
    default:
        return TypeName::Invalid;
    }
}
#endif // DEBUGGING

#endif // THINGS_H
