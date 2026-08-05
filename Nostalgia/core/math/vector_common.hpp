#ifndef NOSTALGIA_VECTOR_COMMON_H
#define NOSTALGIA_VECTOR_COMMON_H

struct __vector
{
    virtual consteval int size() noexcept = 0;
    // virtual consteval
};

#endif // NOSTALGIA_VECTOR_COMMON_H
