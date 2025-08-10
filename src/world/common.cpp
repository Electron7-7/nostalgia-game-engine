#include "3d_common.hpp"
#include <glm/glm.hpp>

// These variables are left unexposed to limit mutation to specific functions
glm::vec3 world_OrientationUp(0.0f, 1.0f, 0.0f);
glm::vec3 world_OrientationRight(1.0f, 0.0f, 0.0f);
glm::vec3 world_OrientationFront(0.0f, 0.0f, -1.0f);

glm::vec3 World::Orientation::Up()
{ return world_OrientationUp; }

glm::vec3 World::Orientation::Right()
{ return world_OrientationRight; }

glm::vec3 World::Orientation::Front()
{ return world_OrientationFront; }

void World::Orientation::SetWorldUp(glm::vec3 new_up)
{ world_OrientationUp = new_up; }

void World::Orientation::SetWorldRight(glm::vec3 new_right)
{ world_OrientationRight = new_right; }

void World::Orientation::SetWorldFront(glm::vec3 new_front)
{ world_OrientationFront = new_front; }