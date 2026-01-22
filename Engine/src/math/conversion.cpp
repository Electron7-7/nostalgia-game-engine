#include "conversion.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "Jolt/Jolt.h" // IWYU pragma: keep
#include "Jolt/Math/MathTypes.h"

template<>
glm::vec3 JoltToGlm(const JPH::Vec3& j)
{ return glm::vec3(j[0], j[1], j[2]); }

template<>
glm::vec4 JoltToGlm(const JPH::Vec4& j)
{ return glm::vec4(j[0], j[1], j[2], j[3]); }

template<>
glm::quat JoltToGlm(const JPH::Quat& j)
{ return glm::quat(j.GetW(), j.GetX(), j.GetY(), j.GetZ()); }

template<>
void JoltToGlm(const JPH::Vec3& j, glm::vec3& g)
{ g = glm::vec3(j[0], j[1], j[2]); }

template<>
void JoltToGlm(const JPH::Vec4& j, glm::vec4& g)
{ g = glm::vec4(j[0], j[1], j[2], j[3]); }

template<>
void JoltToGlm(const JPH::Quat& j, glm::quat& g)
{ g = glm::quat(j.GetW(), j.GetX(), j.GetY(), j.GetZ()); }

template<>
JPH::Vec3 GlmToJolt(const glm::vec3& g)
{ return JPH::Vec3(g[0], g[1], g[2]); }

template<>
JPH::Vec4 GlmToJolt(const glm::vec4& g)
{ return JPH::Vec4(g[0], g[1], g[2], g[3]); }

template<>
JPH::Quat GlmToJolt(const glm::quat& g)
{ glm::quat quat = glm::normalize(g); return JPH::Quat(quat.x, quat.y, quat.z, quat.w); }

template<>
void GlmToJolt(const glm::vec3& g, JPH::Vec3& j)
{ j = JPH::Vec3(g[0], g[1], g[2]); }

template<>
void GlmToJolt(const glm::vec4& g, JPH::Vec4& j)
{ j = JPH::Vec4(g[0], g[1], g[2], g[3]); }

template<>
void GlmToJolt(const glm::quat& g, JPH::Quat& j)
{ glm::quat quat = glm::normalize(g); j = JPH::Quat(quat.x, quat.y, quat.z, quat.w); }
