#include "camera_3d.hpp"
#include "theatre/3d_common.hpp"

void Camera3D::SetupVariables(const data_t& data)
{
    Actor::SetupVariables(data);
}

glm::vec3 Camera3D::GetFront()
{ return Rotation() * World::Orientation::Front(); }

glm::vec3 Camera3D::GetRight()
{ return Rotation() * World::Orientation::Right(); }

glm::vec3 Camera3D::GetUp()
{ return Rotation() * World::Orientation::Up(); }
