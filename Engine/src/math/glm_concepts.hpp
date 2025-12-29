#ifndef GLM_CONCEPTS_H
#define GLM_CONCEPTS_H

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

template<typename T>
    concept GLM_Vec2 = std::same_as<T,glm::vec2>;

template<typename T>
    concept GLM_Vec3 = std::same_as<T,glm::vec3>;

template<typename T>
    concept GLM_Vec4 = std::same_as<T,glm::vec4>;

template<typename T>
    concept GLM_Quat = std::same_as<T,glm::quat>;

template<typename T>
    concept GLMContainer = GLM_Vec2<T> || GLM_Vec3<T> || GLM_Vec4<T> || GLM_Quat<T>;

template<typename T>
    concept NumberOrGLM = std::integral<T> or std::floating_point<T> or GLMContainer<T>;

#endif // GLM_CONCEPTS_H
