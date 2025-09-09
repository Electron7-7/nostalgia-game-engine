#include "camera_3d.hpp"
#include "theatre/3d_common.hpp"

glm::vec3 Camera3D::GetFront()
{ return Rotation() * World::Orientation::Front(); }

glm::vec3 Camera3D::GetRight()
{ return Rotation() * World::Orientation::Right(); }

glm::vec3 Camera3D::GetUp()
{ return Rotation() * World::Orientation::Up(); }
