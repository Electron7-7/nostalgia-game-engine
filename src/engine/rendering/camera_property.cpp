#include "camera_property.hpp"
#include "world/3d_common.hpp"

glm::vec3 CameraProperty::GetFront()
{ return rotation * World::Orientation::Front(); }

glm::vec3 CameraProperty::GetRight()
{ return rotation * World::Orientation::Right(); }

glm::vec3 CameraProperty::GetUp()
{ return rotation * World::Orientation::Up(); }

void CameraProperty::UpdateView()
{
    // TODO: Currently, there's no player so there's nothing for this function to do. In the future, it will get or be given the player
    // and attempt to set its camera's properties to match the player's.
}