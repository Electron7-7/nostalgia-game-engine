#ifndef CONCEPTS_H
#define CONCEPTS_H

#include <concepts>
#include <type_traits>

template<typename T, typename Desired>
    concept is_similar = std::is_same_v<T, Desired> ||
        std::is_convertible_v<T, Desired> ||
        std::is_constructible_v<Desired, T> ||
        std::derived_from<Desired, T>;

#endif // CONCEPTS_H
