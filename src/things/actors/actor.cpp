#include "actor.hpp"
#include "theatre_parser/thing_data.hpp"

void Actor::SetupVariables(const ThingData& data)
{
    Thing::SetupVariables(data); // Currently, this doesn't do anything

    data.GetNumber(mOrigin, "Origin");
    data.GetNumber(mScale, "Scale");
    glm::vec3 euler;
    if(data.GetNumber(euler, "RotationDegrees"))
        { Euler(euler, true); }
    if(data.GetNumber(euler, "Rotation"))
        { Euler(euler); }
    glm::quat quat;
    if(data.GetNumber(quat, "Quaternion"))
        { Quaternion(quat); }
    data.GetReference(mMeshInstanceID, "MeshInstance");
    data.GetBool(mVisible, "Visible");
    data.GetBool(mWireframe, "MakeWireframe");
    data.GetBool(mWireframe, "Wireframe");
}

ID Actor::GetMeshInstanceID() const
{ return mMeshInstanceID; }

void Actor::SetMeshInstanceID(ID mesh_instance_id)
{ mMeshInstanceID = mesh_instance_id; }
