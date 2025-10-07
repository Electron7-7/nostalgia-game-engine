#ifndef HASH_H
#define HASH_H

#include "frozen/string.h"

#include <string>
#include <utility>
#include <type_traits>

typedef unsigned int hash_t;

// https://stackoverflow.com/a/23684632
template<class>struct ConstexprHasher;

template<>
struct ConstexprHasher<frozen::string>
{
    hash_t constexpr operator()(const char* input) const
    {
        return *input ?
            static_cast<unsigned int>(*input) + 33 * (*this)(input + 1) :
            5381;
    }
    hash_t operator()(const frozen::string& string) const
    { return (*this)(string.data()); }
};

template<>
struct ConstexprHasher<std::string>
{
    hash_t constexpr operator()(const char* input) const
    {
        return *input ?
            static_cast<unsigned int>(*input) + 33 * (*this)(input + 1) :
            5381;
    }
    hash_t operator()(const std::string& string) const
    { return (*this)(string.data()); }
};

template<>
struct ConstexprHasher<const char*>
{
    hash_t constexpr operator()(const char* input) const
    {
        return *input ?
            static_cast<unsigned int>(*input) + 33 * (*this)(input + 1) :
            5381;
    }
    hash_t operator()(const frozen::string& string) const
    { return (*this)(string.data()); }
    hash_t operator()(const std::string& string) const
    { return (*this)(string.data()); }
};

template<typename T>
hash_t constexpr ConstexprHash(T&& t)
{ return ConstexprHasher<typename std::decay<T>::type>()(std::forward<T>(t)); }

#endif // HASH_H
