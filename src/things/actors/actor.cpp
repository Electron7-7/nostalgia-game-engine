#include "actor.hpp"
#include "theatre_parser/thing_data.hpp"
#include "glm_to_string.hpp" // IWYU pragma: keep

void Actor::SetVariables(const ThingData& data)
{
    Thing::SetVariables(data); // Currently, this doesn't do anything

    data.GetVariable(mOrigin, "Origin");
    data.GetVariable(mScale, "Scale");
    glm::vec3 euler;
    if(data.GetVariable(euler, "RotationDegrees"))
        { Euler(euler, true); }
    if(data.GetVariable(euler, "Rotation"))
        { Euler(euler); }
    glm::quat quat;
    if(data.GetVariable(quat, "Quaternion"))
        { Quaternion(quat); }
    data.GetVariable(mMeshInstanceID, "MeshInstance");
    data.GetVariable(mVisible, "Visible");
    data.GetVariable(mWireframe, "MakeWireframe");
    data.GetVariable(mWireframe, "Wireframe");
    data.GetVariable(mDebugHighlight, "DebugHighlight");
}

ThingData Actor::GetVariables() const
{
    ThingData data{Thing::GetVariables()};

    data.AddVariable(mOrigin, "Origin");
    data.AddVariable(mScale, "Scale");
    data.AddVariable(mQuaternion, "Quaternion");
    data.AddVariable(mMeshInstanceID, "MeshInstance");
    data.AddVariable(mVisible, "Visible");
    data.AddVariable(mWireframe, "Wireframe");
    data.AddVariable(mDebugHighlight, "DebugHighlight");

    return data;
}

ID Actor::GetMeshInstanceID() const
{ return mMeshInstanceID; }

void Actor::SetMeshInstanceID(ID mesh_instance_id)
{ mMeshInstanceID = mesh_instance_id; }
