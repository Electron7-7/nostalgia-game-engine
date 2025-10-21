#ifndef STRING_CONCEPTS_H
#define STRING_CONCEPTS_H

#include "frozen/string.h"

template<typename T>
    concept StringContainer = requires
    { std::is_same_v<T,std::string> || std::is_same_v<T,frozen::string>; };

template<typename T>
    concept StringType = requires
    {
        StringContainer<T> ||
        std::is_same_v<std::decay_t<T>,std::decay_t<const char*>>;
    };

#endif // STRING_CONCEPTS_H
