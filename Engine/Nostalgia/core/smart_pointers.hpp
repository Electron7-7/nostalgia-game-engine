#ifndef SMART_POINTERS_H
#define SMART_POINTERS_H


#define DCast std::dynamic_pointer_cast
#define SCast std::static_pointer_cast

template<typename T>
    using Unique = std::unique_ptr<T>;

template<typename T, class ...Args>
    constexpr Unique<T> MakeUnique(Args&&... args)
    { return std::make_unique<T>(std::forward<Args>(args)...); }

template<typename T>
    using Shared = std::shared_ptr<T>;

template<typename T, class ...Args>
    constexpr Shared<T> MakeShared(Args&&... args)
    { return std::make_shared<T>(std::forward<Args>(args)...); }

#endif // SMART_POINTERS_H
