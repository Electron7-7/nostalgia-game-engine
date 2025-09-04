#include "typenames.hpp"
#include "hash.hpp"

#include <set>

static const std::set<size_t> s_ResourceTypes =
{
    Type::Mesh,
    Type::Texture,
};

static const std::set<size_t> s_ThingTypes =
{
    Type::Actor,
    Type::NostalgiaPlayer,
    Type::Thinker,
};

const BaseType GetBaseType(const std::string& name)
{ return GetBaseType(ConstexprHash(name)); }

const BaseType GetBaseType(size_t hash)
{
    if(hash == Type::Invalid)
        { return BaseType::Invalid; }
    else if(s_ResourceTypes.contains(hash))
        { return BaseType::Resource; }
    else if(s_ThingTypes.contains(hash))
        { return BaseType::Thing; }

    return BaseType::Invalid;
}
