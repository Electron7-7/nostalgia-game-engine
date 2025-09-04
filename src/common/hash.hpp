#ifndef HASH_H
#define HASH_H

#include <string>
#include <utility>
#include <type_traits>

// https://stackoverflow.com/a/23684632
template<class>struct ConstexprHasher;

template<>
struct ConstexprHasher<std::string>
{
    std::size_t constexpr operator()(char const* input) const
    {
        return *input ?
            static_cast<unsigned int>(*input) + 33 * (*this)(input + 1) :
            5381;
    }

    std::size_t operator()(const std::string& string) const
    { return (*this)(string.c_str()); }
};

template<>
struct ConstexprHasher<const char*>
{
    std::size_t constexpr operator()(char const* input) const
    {
        return *input ?
            static_cast<unsigned int>(*input) + 33 * (*this)(input + 1) :
            5381;
    }

    std::size_t operator()(const std::string& string) const
    { return (*this)(string.c_str()); }
};

template<typename T>
std::size_t constexpr ConstexprHash(T&& t)
{ return ConstexprHasher<typename std::decay<T>::type>()(std::forward<T>(t)); }

#endif // HASH_H
