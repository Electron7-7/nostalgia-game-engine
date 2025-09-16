#include "typenames.hpp"
#include "hash.hpp"

#include <map>

static const std::map<size_t, int> s_TypePriorityMap =
{
    { Type::Mesh,            -2 },
    { Type::Font,            -2 },
    { Type::Texture,         -2 },
    { Type::Material,        -1 },
    { Type::MeshInstance,    -1 },
    { Type::PrototypeActor,   0 },
    { Type::NostalgiaPlayer,  0 },
    { Type::Actor,            0 },
    { Type::Resource,         0 },
    { Type::Thing,            0 },
};

const int g_GetPriority(size_t type)
{
    if(!s_TypePriorityMap.contains(type))
        { return 9999; }
    return s_TypePriorityMap.at(type);
}

const bool g_IsValidType(const std::string& name)
{ return s_TypePriorityMap.contains(ConstexprHash(name)); }

const bool g_IsValidType(size_t hash)
{ return s_TypePriorityMap.contains(hash); }
