#include "typenames.hpp"
#include "hash.hpp"

#include <set>

static const std::set<size_t> s_ResourceTypes =
{
    Type::Mesh,
    Type::Texture,
    Type::Material,
    Type::MeshInstance,
};

static const std::set<size_t> s_ThingTypes =
{
    Type::Actor,
    Type::NostalgiaPlayer,
    Type::PrototypeActor,
};

const size_t GetBaseType(const std::string& name)
{ return GetBaseType(ConstexprHash(name)); }

const size_t GetBaseType(size_t hash)
{
    if(s_ResourceTypes.contains(hash))
        { return Type::Resource; }
    else if(s_ThingTypes.contains(hash))
        { return Type::Thing; }
    return Type::Invalid;
}
