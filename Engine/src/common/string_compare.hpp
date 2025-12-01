#ifndef STRING_EXAMINE_H
#define STRING_EXAMINE_H

#include "string_concepts.hpp"
#include "string_transform.hpp"
#include "frozen/string.h"
#include "core/globals.hpp"

#include <algorithm>

#define FROZEN const frozen::string& Frozen
#define STRING const    std::string& String

constexpr bool operator==(FROZEN, STRING)
{ return !String.compare(Frozen.data()); }

constexpr bool operator!=(FROZEN, STRING)
{ return !(Frozen == String); }

constexpr bool operator< (FROZEN, STRING)
{ return std::lexicographical_compare(Frozen.begin(), Frozen.end(), String.cbegin(), String.cend()); }

constexpr bool operator< (STRING, FROZEN)
{ return std::lexicographical_compare(String.cbegin(), String.cend(), Frozen.begin(), Frozen.end()); }

#undef FROZEN
#undef STRING

constexpr bool gCompareCStrings(const char* Left, const char* Right)
{
    if(Left == Right)
        { return true; }
    else if(Left == nullptr || Right == nullptr)
        { return false; }
    while(*Left != '\0' && *Right != '\0')
    {
        if(*Left != *Right) { return false; }
        ++Left;
        ++Right;
    }
    return (*Left == '\0' && *Right == '\0');
}

template<StringType T>
constexpr bool gCompareToBool(const T& inString, bool CompareTo)
{ return !gGetLowercase(inString).compare((CompareTo) ? gTrue : gFalse); }

template<StringContainer T>
constexpr bool gIsBool(const T& inString)
{
    if(inString.size() < 4 || inString.size() > 5)
        { return false; }
    T lower{gGetLowercase(inString)};
    return gCompareToBool(lower, true) || gCompareToBool(lower, false);
}

#endif // STRING_EXAMINE_H
