#include "theatre_parsing_lookups.hpp"

#include <string>
#include <set>

static const std::set<std::string> s_ResourceTypes =
{
    "Resource",
    "MeshInstance",
    "Material",
    "Texture",
};

static const std::set<std::string> s_ThingTypes =
{
    "Thing",
    "Actor",
    "Thinker",
    "NostalgiaPlayer",
    "PrototypeActor",
};

BaseType GetBaseType(const std::string& type_name)
{
    if(s_ResourceTypes.contains(type_name))
        { return BaseType::Resource; }
    else if(s_ThingTypes.contains(type_name))
        { return BaseType::Thing; }
    return BaseType::Unknown;
}
