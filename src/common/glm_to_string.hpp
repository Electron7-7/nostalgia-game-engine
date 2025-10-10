#ifndef GLM_TO_STRING_H
#define GLM_TO_STRING_H

#include "glm/glm.hpp" // IWYU pragma: keep
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/quaternion.hpp" // IWYU pragma: keep

#include <format>

template<typename T>
concept GLMContainer = requires
{
    (
        std::is_same_v<std::decay_t<glm::vec2>, std::decay_t<T>> ||
        std::is_same_v<std::decay_t<glm::vec3>, std::decay_t<T>> ||
        std::is_same_v<std::decay_t<glm::vec4>, std::decay_t<T>> ||
        std::is_same_v<std::decay_t<glm::quat>, std::decay_t<T>>
    ) == true;
};

template<>
struct std::formatter<glm::vec2> : std::formatter<std::string>
{
    auto format(const glm::vec2& vec2, std::format_context& ctx) const
    { return std::formatter<std::string>::format(std::format("{},{}", vec2[0], vec2[1]), ctx); }
};

template<>
struct std::formatter<glm::vec3> : std::formatter<std::string>
{
    auto format(const glm::vec3& vec3, std::format_context& ctx) const
    { return std::formatter<std::string>::format(std::format("{},{},{}", vec3[0], vec3[1], vec3[2]), ctx); }
};

template<>
struct std::formatter<glm::vec4> : std::formatter<std::string>
{
    auto format(const glm::vec4& vec4, std::format_context& ctx) const
    { return std::formatter<std::string>::format(std::format("{},{},{},{}", vec4[0], vec4[1], vec4[2], vec4[3]), ctx); }
};

template<>
struct std::formatter<glm::quat> : std::formatter<std::string>
{
    auto format(const glm::quat& quat, std::format_context& ctx) const
    { return std::formatter<std::string>::format(std::format("{},{},{},{}", quat[0], quat[1], quat[2], quat[3]), ctx); }
};

#endif // GLM_TO_STRING_H
