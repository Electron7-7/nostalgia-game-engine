#ifndef MATH_CONCEPTS_H
#define MATH_CONCEPTS_H

#include <concepts>

template<typename T>
    concept IsEnum = std::is_enum_v<T> or std::is_scoped_enum_v<T>;

// Math Concepts
template<typename T>
    concept NumberOrBool = std::integral<T> or std::floating_point<T>;

template<typename T>
    concept Number = NumberOrBool<T> and !(std::same_as<T, bool>);

#endif // MATH_CONCEPTS_H
