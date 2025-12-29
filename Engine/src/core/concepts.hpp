#ifndef COMMON_CONCEPTS_H
#define COMMON_CONCEPTS_H

#include "thirdparty/frozen/string.h"
#include <string>
#include <concepts>

// Common Concepts
template<typename From, typename To>
    concept can_become = std::derived_from<From,To> || std::is_convertible_v<From,To>;

// String Concepts
template<typename T>
    concept StringContainer = std::same_as<T, std::string> || std::same_as<T, frozen::string>;

template<typename T>
    concept StringType = std::convertible_to<T, std::string> || std::convertible_to<T, frozen::string>;

#endif // COMMON_CONCEPTS_H
