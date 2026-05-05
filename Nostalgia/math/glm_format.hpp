#ifndef GLM_FORMAT_H
#define GLM_FORMAT_H

template<>
struct std::formatter<glm::vec2> : std::formatter<std::string>
{
    auto format(const glm::vec2& vec2, std::format_context& ctx) const
    { return std::formatter<std::string>::format(std::format("{:f}, {:f}", vec2[0], vec2[1]), ctx); }
};

template<>
struct std::formatter<glm::vec3> : std::formatter<std::string>
{
    auto format(const glm::vec3& vec3, std::format_context& ctx) const
    { return std::formatter<std::string>::format(std::format("{:f}, {:f}, {:f}", vec3[0], vec3[1], vec3[2]), ctx); }
};

template<>
struct std::formatter<glm::vec4> : std::formatter<std::string>
{
    auto format(const glm::vec4& vec4, std::format_context& ctx) const
    { return std::formatter<std::string>::format(std::format("{:f}, {:f}, {:f}, {:f}", vec4[0], vec4[1], vec4[2], vec4[3]), ctx); }
};

template<>
struct std::formatter<glm::quat> : std::formatter<std::string>
{
    auto format(const glm::quat& quat, std::format_context& ctx) const
    { return std::formatter<std::string>::format(std::format("{:f}, {:f}, {:f}, {:f}", quat[0], quat[1], quat[2], quat[3]), ctx); }
};

#endif // GLM_FORMAT_H
