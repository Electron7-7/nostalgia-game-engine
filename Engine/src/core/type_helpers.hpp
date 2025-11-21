#ifndef TYPE_HELPERS_H
#define TYPE_HELPERS_H

#include <memory>

#define MAKE_TYPE_ARG(STRUCT_NAME, STRUCT_DEFINITION, OPTIONAL_CSTYLE_VARIABLE) \
struct STRUCT_NAME { \
    STRUCT_DEFINITION \
} OPTIONAL_CSTYLE_VARIABLE; \
typedef const STRUCT_NAME& STRUCT_NAME##Arg;

#define FARG(TYPE) const TYPE&

// Stolen from Hazel
template<typename T>
    using Unique = std::unique_ptr<T>;

template<typename T, class ...Args>
    constexpr Unique<T> MakeUnique(Args&&... args)
    { return std::make_unique<T>(std::forward<Args>(args)...); }

// Stolen from Hazel
template<typename T>
    using Shared = std::shared_ptr<T>;

template<typename T, class ...Args>
    constexpr Unique<T> MakeShared(Args&&... args)
    { return std::make_shared<T>(std::forward<Args>(args)...); }

#endif // TYPE_HELPERS_H
