#ifndef HASH_H
#define HASH_H

#include "frozen/string.h"

#include <string>
#include <type_traits>

// https://stackoverflow.com/a/23684632
template<class>
    struct ConstexprHasher;

template<>
struct ConstexprHasher<frozen::string>
{
    uint constexpr operator()(const char* input) const
    {
        return *input ?
            static_cast<unsigned int>(*input) + 33 * (*this)(input + 1) :
            5381;
    }
    uint operator()(const frozen::string& string) const
    { return (*this)(string.data()); }
};

template<>
struct ConstexprHasher<std::string>
{
    uint constexpr operator()(const char* input) const
    {
        return *input ?
            static_cast<unsigned int>(*input) + 33 * (*this)(input + 1) :
            5381;
    }
    uint operator()(const std::string& string) const
    { return (*this)(string.data()); }
};

template<>
struct ConstexprHasher<const char*>
{
    uint constexpr operator()(const char* input) const
    {
        return *input ?
            static_cast<unsigned int>(*input) + 33 * (*this)(input + 1) :
            5381;
    }
    uint operator()(const frozen::string& string) const
    { return (*this)(string.data()); }
    uint operator()(const std::string& string) const
    { return (*this)(string.data()); }
};

template<typename T>
uint constexpr ConstexprHash(T&& t)
{ return ConstexprHasher<typename std::decay<T>::type>()(std::forward<T>(t)); }

#endif // HASH_H
