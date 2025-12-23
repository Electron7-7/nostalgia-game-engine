#include "enum_prettifier.hpp"

Error EnumPrettifier::EraseEnum(Farg<std::string> inName, uint inEnumSet)
{
    if(auto found_it{m_sEnumMaps.find(inEnumSet)}; found_it != m_sEnumMaps.end())
    {
        for(auto& [num, name] : found_it->second)
        {
            if(!inName.compare(name))
            {
                found_it->second.erase(num);
                return OK;
            }
        }
    }
    return ERR_NOT_FOUND;
}

Error EnumPrettifier::EraseSet(uint inEnumSet)
{
    return (m_sEnumMaps.erase(inEnumSet))
        ? OK
        : ERR_NOT_FOUND;
}

void EnumPrettifier::Clear()
{
    m_sEnumMaps.clear();
}

Error EnumPrettifier::EraseEnum(Farg<std::string> inName, Farg<std::string> inEnumSet)
{ return EraseEnum(inName, ConstexprHash(inEnumSet)); }

Error EnumPrettifier::EraseSet(Farg<std::string> inEnumSet)
{ return EraseSet(ConstexprHash(inEnumSet)); }
