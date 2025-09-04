#ifndef TYPENAMES_H
#define TYPENAMES_H

#include "hash.hpp"

namespace TypeName
{
    constexpr const char* Invalid         = "Invalid";
    constexpr const char* Resource        = "Resource";
    constexpr const char* Mesh            = "Mesh";
    constexpr const char* Texture         = "Texture";
    constexpr const char* Thing           = "Thing";
    constexpr const char* Actor           = "Actor";
    constexpr const char* NostalgiaPlayer = "NostalgiaPlayer";
    constexpr const char* Thinker         = "Thinker";
}

namespace Type
{
    constexpr size_t Invalid         = ConstexprHash(TypeName::Invalid);
    constexpr size_t Resource        = ConstexprHash(TypeName::Resource);
    constexpr size_t Mesh            = ConstexprHash(TypeName::Mesh);
    constexpr size_t Texture         = ConstexprHash(TypeName::Texture);
    constexpr size_t Thing           = ConstexprHash(TypeName::Thing);
    constexpr size_t Actor           = ConstexprHash(TypeName::Actor);
    constexpr size_t NostalgiaPlayer = ConstexprHash(TypeName::NostalgiaPlayer);
    constexpr size_t Thinker         = ConstexprHash(TypeName::Thinker);
}

enum class BaseType
{
    Resource,
    Thing,
    Invalid
};

const BaseType GetBaseType(const std::string& TypeName);
const BaseType GetBaseType(size_t Type);

#endif // TYPENAMES_H
