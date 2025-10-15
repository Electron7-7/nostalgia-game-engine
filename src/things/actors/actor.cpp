#include "actor.hpp"
#include "theatre_parser/thing_data.hpp"
#include "glm_to_string.hpp" // IWYU pragma: keep

void Actor::SetVariables(const ThingData& data)
{
    Thing::SetVariables(data);
    SetTransformVariables(data);

    data.GetVariable(mMeshInstanceID, "MeshInstance");
    data.GetVariable(mColliderID, "Collider");
    data.GetVariable(mVisible, "Visible");
    data.GetVariable(mWireframe, "MakeWireframe");
    data.GetVariable(mWireframe, "Wireframe");
    data.GetVariable(mDebugHighlight, "DebugHighlight");
}

ThingData Actor::GetVariables() const
{
    ThingData data{Thing::GetVariables()};
    GetTransformVariables(data);

    data.AddVariable(mMeshInstanceID, "MeshInstance");
    data.AddVariable(mColliderID, "Collider");
    data.AddVariable(mVisible, "Visible");
    data.AddVariable(mWireframe, "Wireframe");
    data.AddVariable(mDebugHighlight, "DebugHighlight");

    return data;
}

ID Actor::GetMeshInstanceID() const
{ return mMeshInstanceID; }

void Actor::SetMeshInstanceID(ID mesh_instance_id)
{ mMeshInstanceID = mesh_instance_id; }

ID Actor::ColliderID() const
{ return mColliderID; }

void Actor::ColliderID(ID uid)
{ mColliderID = uid; }
