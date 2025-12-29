#ifndef STRING_COMPARE_H
#define STRING_COMPARE_H

#include "frozen/string.h"
#include <string>
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

#undef FROZEN
#undef STRING
#endif // STRING_COMPARE_H
