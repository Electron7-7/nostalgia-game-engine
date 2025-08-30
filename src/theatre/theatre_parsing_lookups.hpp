#ifndef THEATRE_PARSING_LOOKUPS_H
#define THEATRE_PARSING_LOOKUPS_H

#include <string>

enum class BaseType
{
    Unknown,
    Resource,
    Thing
};

BaseType GetBaseType(const std::string& TypeName);

#endif // THEATRE_PARSING_LOOKUPS_H
