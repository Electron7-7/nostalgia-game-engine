#include "actor.hpp"
#include "managers/theatre_manager.hpp"
#include "things/devices/collider.hpp"
#include "theatre/parser/thing_data.hpp"

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

void Actor::Ready()
{
    if(auto collider = g_pTheatreManager->GetThing<Collider>(mColliderID);
        !collider->BodyIDInvalid())
        { mScale = collider->Scale(); }
}

void Actor::Tick()
{
    if(mColliderID == ID::Invalid)
        { return; }
    SetOrigin(g_pTheatreManager->GetThing<Collider>(mColliderID)->Origin());
    SetQuaternion(g_pTheatreManager->GetThing<Collider>(mColliderID)->Quaternion());
}

ID Actor::MeshInstanceID() const
{ return mMeshInstanceID; }

void Actor::MeshInstanceID(ID mesh_instance_id)
{ mMeshInstanceID = mesh_instance_id; }

ID Actor::ColliderID() const
{ return mColliderID; }

void Actor::ColliderID(ID uid)
{ mColliderID = uid; }
