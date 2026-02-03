#include "conversion.hpp"
#include <Jolt/Jolt.h>
#include <Jolt/Math/MathTypes.h>

template<>
glm::vec3 Math::Convert(Farg<JPH::Vec3> j) noexcept
{ return glm::vec3(j[0], j[1], j[2]); }

template<>
glm::vec4 Math::Convert(Farg<JPH::Vec4> j) noexcept
{ return glm::vec4(j[0], j[1], j[2], j[3]); }

template<>
glm::quat Math::Convert(Farg<JPH::Quat> j) noexcept
{ return glm::quat(j.GetW(), j.GetX(), j.GetY(), j.GetZ()); }

template<>
void Math::Convert(glm::vec3& g, Farg<JPH::Vec3> j) noexcept
{ g = glm::vec3(j[0], j[1], j[2]); }

template<>
void Math::Convert(glm::vec4& g, Farg<JPH::Vec4> j) noexcept
{ g = glm::vec4(j[0], j[1], j[2], j[3]); }

template<>
void Math::Convert(glm::quat& g, Farg<JPH::Quat> j) noexcept
{ g = glm::quat(j.GetW(), j.GetX(), j.GetY(), j.GetZ()); }

template<>
JPH::Vec3 Math::Convert(Farg<glm::vec3> g) noexcept
{ return JPH::Vec3(g[0], g[1], g[2]); }

template<>
JPH::Vec4 Math::Convert(Farg<glm::vec4> g) noexcept
{ return JPH::Vec4(g[0], g[1], g[2], g[3]); }

template<>
JPH::Quat Math::Convert(Farg<glm::quat> g) noexcept
{ glm::quat quat = glm::normalize(g); return JPH::Quat(quat.x, quat.y, quat.z, quat.w); }

template<>
void Math::Convert(JPH::Vec3& j, Farg<glm::vec3> g) noexcept
{ j = JPH::Vec3(g[0], g[1], g[2]); }

template<>
void Math::Convert(JPH::Vec4& j, Farg<glm::vec4> g) noexcept
{ j = JPH::Vec4(g[0], g[1], g[2], g[3]); }

template<>
void Math::Convert(JPH::Quat& j, Farg<glm::quat> g) noexcept
{ glm::quat quat = glm::normalize(g); j = JPH::Quat(quat.x, quat.y, quat.z, quat.w); }
