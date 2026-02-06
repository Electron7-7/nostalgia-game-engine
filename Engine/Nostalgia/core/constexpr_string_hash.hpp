#ifndef STRING_HASH_H
#define STRING_HASH_H

#ifdef NOSTALGIA_GCC_PCH
#   include <Nostalgia/Nostalgia.hpp>
#endif

// https://stackoverflow.com/a/23684632
template<class>
    struct ConstexprHasher;

template<>
    struct ConstexprHasher<std::string>
    {
        constexpr uint operator()(const char* input) const
        {
            return *input ?
                static_cast<uint>(*input) + 33 * (*this)(input + 1) :
                5381;
        }

        constexpr uint operator()(const std::string& string) const
        { return (*this)(string.data()); }
    };

template<>
    struct ConstexprHasher<const char*>
    {
        constexpr uint operator()(const char* input) const
        {
            return (*input)
                ? static_cast<uint>(*input) + 33 * (*this)(input + 1)
                : 5381;
        }

        constexpr uint operator()(const std::string& string) const
        { return (*this)(string.data()); }
    };

template<typename T>
    constexpr uint ConstexprHash(T&& t)
    { return ConstexprHasher<typename std::decay<T>::type>()(std::forward<T>(t)); }

#endif // STRING_HASH_H
