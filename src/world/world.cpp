#include "world.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

static glm::vec3 s_GlobalOrientationUp(0.0f, 1.0f, 0.0f);
static glm::vec3 s_GlobalOrientationRight(1.0f, 0.0f, 0.0f);
static glm::vec3 s_GlobalOrientationFront(0.0f, 0.0f, -1.0f);

const glm::vec3& World::Up()
{ return s_GlobalOrientationUp; }

const glm::vec3& World::Right()
{ return s_GlobalOrientationRight; }

const glm::vec3& World::Front()
{ return s_GlobalOrientationFront; }

void World::SetUp(const glm::vec3& up)
{ s_GlobalOrientationUp = up; }

void World::SetRight(const glm::vec3& right)
{ s_GlobalOrientationRight = right; }

void World::SetFront(const glm::vec3& front)
{ s_GlobalOrientationFront = front; }
