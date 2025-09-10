#ifndef TYPENAMES_H
#define TYPENAMES_H

#include "hash.hpp"

namespace TypeName
{
    constexpr const char* Invalid         = "Invalid";
    constexpr const char* Resource        = "Resource";
    constexpr const char* Mesh            = "Mesh";
    constexpr const char* Texture         = "Texture";
    constexpr const char* Material        = "Material";
    constexpr const char* MeshInstance    = "MeshInstance";
    constexpr const char* Thing           = "Thing";
    constexpr const char* Actor           = "Actor";
    constexpr const char* PrototypeActor  = "PrototypeActor";
    constexpr const char* NostalgiaPlayer = "NostalgiaPlayer";
}

namespace Type
{
    constexpr size_t Invalid         = ConstexprHash(TypeName::Invalid);
    constexpr size_t Resource        = ConstexprHash(TypeName::Resource);
    constexpr size_t Mesh            = ConstexprHash(TypeName::Mesh);
    constexpr size_t Texture         = ConstexprHash(TypeName::Texture);
    constexpr size_t Material        = ConstexprHash(TypeName::Material);
    constexpr size_t MeshInstance    = ConstexprHash(TypeName::MeshInstance);
    constexpr size_t Thing           = ConstexprHash(TypeName::Thing);
    constexpr size_t Actor           = ConstexprHash(TypeName::Actor);
    constexpr size_t PrototypeActor  = ConstexprHash(TypeName::PrototypeActor);
    constexpr size_t NostalgiaPlayer = ConstexprHash(TypeName::NostalgiaPlayer);
}

const size_t GetBaseType(const std::string& TypeName);
const size_t GetBaseType(size_t Type);

#ifdef DEBUGGING
inline const char* const& StringifyType(size_t Type)
{
    switch(Type)
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
    case Type::PrototypeActor:
        return TypeName::PrototypeActor;
    case Type::NostalgiaPlayer:
        return TypeName::NostalgiaPlayer;
    default:
        return TypeName::Invalid;
    }
}
#endif // DEBUGGING

#endif // TYPENAMES_H
