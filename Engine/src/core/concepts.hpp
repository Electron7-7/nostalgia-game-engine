#ifndef COMMON_CONCEPTS_H
#define COMMON_CONCEPTS_H

#include <string>
#include <concepts>

// Math Concepts
template<typename T>
    concept NumberOrBool = std::integral<T> or std::floating_point<T>;

template<typename T>
    concept Number = NumberOrBool<T> and !(std::same_as<T, bool>);

// Common Concepts
template<typename From, typename To>
    concept can_become = std::derived_from<From,To> || std::is_convertible_v<From,To>;

template<typename T>
    concept IsEnum = std::is_enum_v<T> or std::is_scoped_enum_v<T>;

// String Concepts
template<typename T>
    concept StringContainer = std::same_as<T, std::string>;

template<typename T>
    concept StringType = std::convertible_to<T, std::string>;

#endif // COMMON_CONCEPTS_H
