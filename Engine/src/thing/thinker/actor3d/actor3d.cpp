#include "actor3d.hpp"
#include "managers/theatre_manager.hpp"
#include "collider3d.hpp"
#include "thing/thing_factory.hpp"
#include "theatre/parser/thing_data.hpp"

void Actor3D::SetVariables(Farg<ThingData> data)
{
    Thinker::SetVariables(data);
    SetTransformVariables(data);

    data.GetVariable(mDebugMeshInstanceID, "DebugMeshInstance");
    data.GetVariable(mVisible, "Visible");
    data.GetVariable(mWireframe, "MakeWireframe");
    data.GetVariable(mWireframe, "Wireframe");
    data.GetVariable(mDebugHighlight, "DebugHighlight");
}

Shared<ThingData> Actor3D::GetVariables() const
{
    auto data{Thinker::GetVariables()};
    GetTransformVariables(data);

    data->AddVariable(mDebugMeshInstanceID, "DebugMeshInstance");
    data->AddVariable(mVisible, "Visible");
    data->AddVariable(mWireframe, "Wireframe");
    data->AddVariable(mDebugHighlight, "DebugHighlight");

    return data;
}

void Actor3D::Ready()
{ Thinker::Ready(); }

void Actor3D::Tick()
{
    for(auto child : mChildren)
    {
        if(g_pThingFactory->IsDerivedFrom<Collider>(child.type))
        {
            auto collider{DCast<Transform3D>(g_pTheatreManager->GetThing<Collider>(child.id))};
            Origin(collider->Origin());
            Scale(collider->Scale());
            Euler(collider->Euler());
        }
    }
}

ID Actor3D::DebugMeshInstance() const
{ return mDebugMeshInstanceID; }

void Actor3D::DebugMeshInstance(ID inID)
{ mDebugMeshInstanceID = inID; }

bool Actor3D::Visible() const
{ return mVisible; }

void Actor3D::Visible(bool isVisible)
{ mVisible = isVisible; }

bool Actor3D::Wireframe() const
{ return mWireframe; }

void Actor3D::Wireframe(bool isWireframe)
{ mWireframe = isWireframe; }
