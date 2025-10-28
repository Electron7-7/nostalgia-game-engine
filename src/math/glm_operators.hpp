#ifndef GLM_OPERATORS_H
#define GLM_OPERATORS_H

#include <math/glm_concepts.hpp>

template<typename T> requires std::is_arithmetic_v<T>
glm::vec3 operator*(const T& lhs, const glm::vec3& rhs)
{
    return
    {
        lhs * rhs[0],
        lhs * rhs[1],
        lhs * rhs[2]
    };
}

template<typename T> requires std::is_arithmetic_v<T>
glm::vec2 operator*(const T& lhs, const glm::vec2& rhs)
{
    return
    {
        lhs * rhs[0],
        lhs * rhs[1]
    };
}

template<typename T> requires std::is_arithmetic_v<T>
glm::vec2 operator*(const T& lhs, const glm::vec4& rhs)
{
    return
    {
        lhs * rhs[0],
        lhs * rhs[1],
        lhs * rhs[2],
        lhs * rhs[3]
    };
}

template<typename T> requires std::is_arithmetic_v<T>
glm::vec2 operator*(const T& lhs, const glm::quat& rhs)
{
    return
    {
        lhs * rhs[0],
        lhs * rhs[1],
        lhs * rhs[2],
        lhs * rhs[3]
    };
}

#endif // GLM_OPERATORS_H
