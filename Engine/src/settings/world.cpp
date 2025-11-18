#include "world.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

using namespace Settings;

static glm::vec3 sOrientationUp{0.0f, 1.0f, 0.0f};
static glm::vec3 sOrientationRight{1.0f, 0.0f, 0.0f};
static glm::vec3 sOrientationFront{0.0f, 0.0f, -1.0f};

const glm::vec3& World::Up()
{ return sOrientationUp; }

const glm::vec3& World::Right()
{ return sOrientationRight; }

const glm::vec3& World::Front()
{ return sOrientationFront; }
