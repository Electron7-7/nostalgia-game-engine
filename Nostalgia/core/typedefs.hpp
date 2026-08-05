#ifndef NOSTALGIA_TYPEDEFS_H
#define NOSTALGIA_TYPEDEFS_H

// Based on Godot's 'typedefs.h'
// https://github.com/godotengine/godot/tree/4cd2d92ae0fe7b2bd9ccab97178daa82911b1d70/core/typedefs.h

#include "core/error/error_list.hpp" // IWYU pragma: keep
#include <cstdint>

using int64_t  = std::int64_t;
using int32_t  = std::int32_t;
using uint64_t = std::uint64_t;
using uint32_t = std::uint32_t;

// "FARG" and "FAUTO" were hold-overs from old Nostalgia (which are hold-overs from GraphX)
// "FARG" stands for "fast argument", since it used to be a preprocessor definition that I'd use as shorthand for const ref
// function arguments; the habit's stuck since then (and I find the word "farg" to be funny)
// "FAUTO" is just "FARG(auto)"
template<typename T>
    using Farg = const T&;

#define FAUTO const auto&;

#ifndef __STR
#   define __STR(inString) #inString
#   define __MKSTR(inString) __STR(inString)
#endif // __STR

#ifndef __ALWAYS_INLINE__
#   ifdef __GNUC__
#      define __ALWAYS_INLINE__ __attribute__((always_inline)) inline
#   elifdef __clang__
#      define __ALWAYS_INLINE__ [[clang::always_inline]]
#   elifdef _MSC_VER_
#      define __ALWAYS_INLINE__ __forceinline
#   else
#      define __ALWAYS_INLINE__ inline
#   endif
#endif // __ALWAYS_INLINE__

#undef SIGN
#undef MIN
#undef MAX
#undef CLAMP

template<typename T>
    constexpr const T SIGN(const T inValue)
    { return inValue > 0 ? +1.0f : (inValue < 0 ? -1.0f : 0.0f); }

template<typename T, typename T2>
    constexpr auto MIN(const T inA, const T2 inB)
    { return inA < inB ? inA : inB; }

template<typename T, typename T2>
    constexpr auto MAX(const T inA, const T2 inB)
    { return inA > inB ? inA : inB; }

template<typename T, typename T2, typename T3>
    constexpr auto CLAMP(const T inValue, const T2 inMin, const T3 inMax)
    { return inValue < inMin ? inMin : (inValue > inMax ? inMax : inValue); }

#endif // NOSTALGIA_TYPEDEFS_H
