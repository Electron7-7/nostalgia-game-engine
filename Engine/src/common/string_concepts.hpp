#ifndef STRING_CONCEPTS_H
#define STRING_CONCEPTS_H

#include "concepts.hpp"
#include "frozen/string.h"

template<typename T>
    concept StringContainer = !std::is_same_v<T, std::nullptr_t> &&
        (is_similar<T, std::string> || is_similar<T, frozen::string>);

template<typename T>
    concept StringType = !std::is_same_v<T, std::nullptr_t> &&
        (is_similar<T, const char*> || StringContainer<T>);

#endif // STRING_CONCEPTS_H
