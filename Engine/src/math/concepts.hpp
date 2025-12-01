#ifndef MATH_CONCEPTS_H
#define MATH_CONCEPTS_H

#include <type_traits>

template<typename T>
    concept Number = std::is_arithmetic_v<T> && !std::is_same_v<T, std::nullptr_t>;

template<typename T>
    concept Scalar = std::is_scalar_v<T> && !std::is_same_v<T, std::nullptr_t>;

#endif // MATH_CONCEPTS_H
