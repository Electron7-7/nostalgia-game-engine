#ifndef CONTAINERS_H
#define CONTAINERS_H

#include "concepts.hpp" // IWYU pragma: keep
#include "thirdparty/DearImGui/imgui.h" // IWYU pragma: keep
#include "thirdparty/for_each_macro.hpp"
#include "thirdparty/va_args_count.h"

#include <cassert>
#include <sys/types.h>

#define SCALAR(NAME) T NAME{0};
#define ARG(NAME) R NAME = 0,
#define LIST(NAME) NAME{static_cast<T>(NAME)},
#define EQUAL(NAME) NAME == other.NAME &&
#define OPERATOR_ADD_ASSIGN(NAME) NAME += static_cast<T>(other.NAME);
#define OPERATOR_ADD(NAME) static_cast<T>(NAME + other.NAME),
#define OPERATOR_SUB_ASSIGN(NAME) NAME -= static_cast<T>(other.NAME);
#define OPERATOR_SUB(NAME) static_cast<T>(NAME - other.NAME),

#define VECTOR_TEMPLATE(NAME, VARIABLES...) \
    template<Number T> \
        struct NAME \
        { \
            enum class Origin { \
                TopLeft, \
                TopRight, \
                BottomLeft, \
                BottomRight, \
            }; \
 \
            FOR_EACH(SCALAR, VARIABLES) \
            Origin origin{Origin::TopLeft}; \
 \
            constexpr NAME() = default; \
 \
            template<typename R> requires std::is_convertible_v<R, T> \
                constexpr NAME(FOR_EACH(ARG, VARIABLES) Origin Origin = Origin::TopLeft): \
                    FOR_EACH(LIST, VARIABLES) origin{Origin} {} \
 \
            constexpr bool equal_to(const NAME& other) const \
            { return FOR_EACH(EQUAL, VARIABLES) true; } \
 \
            constexpr bool operator==(const NAME& other) const \
            { return equal_to(other) && origin == other.origin; } \
 \
            constexpr const NAME& operator-=(const NAME& other) \
            { \
                FOR_EACH(OPERATOR_SUB_ASSIGN, VARIABLES) \
                return *this; \
            } \
 \
            constexpr NAME operator-(const NAME& other) const \
            { return NAME{ FOR_EACH(OPERATOR_SUB, VARIABLES) }; } \
 \
            constexpr const NAME& operator+=(const NAME& other) \
            { \
                FOR_EACH(OPERATOR_ADD_ASSIGN, VARIABLES) \
                return *this; \
            } \
 \
            constexpr NAME operator+(const NAME& other) const \
            { return NAME{ FOR_EACH(OPERATOR_ADD, VARIABLES) }; } \
        };

struct ImVec2;

namespace
{
    VECTOR_TEMPLATE(vector2_xy, x, y)
    VECTOR_TEMPLATE(vector2_wh, width, height)
}

typedef vector2_xy<int>    iPosition;
typedef vector2_xy<long>   lPosition;
typedef vector2_xy<float>  fPosition;
typedef vector2_xy<double> dPosition;
typedef vector2_xy<double>  Position;

typedef vector2_xy<int>    iMotion;
typedef vector2_xy<long>   lMotion;
typedef vector2_xy<float>  fMotion;
typedef vector2_xy<double> dMotion;
typedef vector2_xy<double>  Motion;

typedef vector2_wh<double> dScale;
typedef vector2_wh<float>  fScale;
typedef vector2_wh<long>   lScale;
typedef vector2_wh<int>    iScale;
typedef vector2_wh<int>     Scale;

#undef VECTOR2_TEMPLATE

#endif // CONTAINERS_H
