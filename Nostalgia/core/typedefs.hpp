#ifndef NOSTALGIA_TYPEDEFS_H
#define NOSTALGIA_TYPEDEFS_H

// Redefining common type shorthands (bc Windows is old and he... he forgets things sometimes, okay, kid?)
using uint32 = unsigned int;
using uint64 = unsigned long;
using uint   = uint32;
using ushort = unsigned short;
using uchar  = unsigned char;

// Stolen from [Godot](core/typedefs.h#L348) and edited to include Clang
#if defined(__clang__)
#   define likely(x) __builtin_expect(!!(x), 1)
#   define unlikely(x) __builtin_expect(!!(x), 0)
#   define how_likely(x, prob) __builtin_expect_with_probability(!!(x), 1, prob) // how hungry...?
#   define how_unlikely(x, prob) __builtin_expect_with_probability(!!(x), 0, prob)
#elif defined(__GNUC__)
#   define likely(x) __builtin_expect(!!(x), 1)
#   define unlikely(x) __builtin_expect(!!(x), 0)
#   define how_likely(x, prob) __builtin_expect(!!(x), 1)
#   define how_unlikely(x, prob) __builtin_expect(!!(x), 0)
#else
#   define likely(x) x
#   define unlikely(x) x
#   define how_likely(x, prob) x
#   define how_unlikely(x, prob) x
#endif

#endif // NOSTALGIA_TYPEDEFS_H
