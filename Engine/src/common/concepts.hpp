#ifndef CONCEPTS_H
#define CONCEPTS_H

#include <concepts>
#include <type_traits>

template<typename From, typename To>
    concept can_become = std::derived_from<From,To> || std::is_convertible_v<From,To>;

#endif // CONCEPTS_H
