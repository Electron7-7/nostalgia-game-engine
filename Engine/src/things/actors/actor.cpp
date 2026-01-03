#include "actor.hpp"
#include "managers/theatre_manager.hpp"
#include "things/devices/collider.hpp"
#include "theatre/parser/thing_data.hpp"

void __actor_t::SetVariables(Farg<ThingData> data)
{
    Thing::SetVariables(data);
    SetTransformVariables(data);

    data.GetVariable(mDebugMeshInstanceID, "DebugMeshInstance");
    data.GetVariable(mVisible, "Visible");
    data.GetVariable(mWireframe, "MakeWireframe");
    data.GetVariable(mWireframe, "Wireframe");
    data.GetVariable(mDebugHighlight, "DebugHighlight");
}

Shared<ThingData> __actor_t::GetVariables() const
{
    auto data{Thing::GetVariables()};
    GetTransformVariables(data);

    data->AddVariable(mDebugMeshInstanceID, "DebugMeshInstance");
    data->AddVariable(mVisible, "Visible");
    data->AddVariable(mWireframe, "Wireframe");
    data->AddVariable(mDebugHighlight, "DebugHighlight");

    return data;
}

void __actor_t::Ready()
{
    Thing::Ready();
    for(auto child : mChildren)
    {
        if(auto collider{g_pTheatreManager->GetThing<Collider>(child.id)}; !collider->BodyIDInvalid())
            { mScale = collider->Scale(); }
    }
}

void __actor_t::Tick()
{
    for(auto child : mChildren)
    {
        if(auto collider{g_pTheatreManager->GetThing<Collider>(child.id)}; !collider->uid().invalid())
        {
            SetOrigin(collider->Origin());
            SetQuaternion(collider->Quaternion());
        }
    }
}

ID __actor_t::DebugMeshInstance() const
{ return mDebugMeshInstanceID; }

void __actor_t::DebugMeshInstance(ID inID)
{ mDebugMeshInstanceID = inID; }

bool __actor_t::Visible() const
{ return mVisible; }

void __actor_t::Visible(bool isVisible)
{ mVisible = isVisible; }

bool __actor_t::Wireframe() const
{ return mWireframe; }

void __actor_t::Wireframe(bool isWireframe)
{ mWireframe = isWireframe; }
