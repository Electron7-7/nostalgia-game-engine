#ifndef GLM_CONCEPTS_H
#define GLM_CONCEPTS_H

#include "glm/glm.hpp" // IWYU pragma: keep
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/quaternion.hpp" // IWYU pragma: keep

template<typename T>
    concept GLM_Vec2 = std::is_same_v<std::decay_t<T>,std::decay_t<glm::vec2>>;

template<typename T>
    concept GLM_Vec3 = std::is_same_v<std::decay_t<T>,std::decay_t<glm::vec3>>;

template<typename T>
    concept GLM_Vec4 = std::is_same_v<std::decay_t<T>,std::decay_t<glm::vec4>>;

template<typename T>
    concept GLM_Quat = std::is_same_v<std::decay_t<T>,std::decay_t<glm::quat>>;

template<typename T>
    concept GLMContainer = GLM_Vec2<T> || GLM_Vec3<T> || GLM_Vec4<T> || GLM_Quat<T>;

template<typename T>
    concept NumberOrGLM = std::integral<T> or std::floating_point<T> or GLMContainer<T>;

#endif // GLM_CONCEPTS_H
