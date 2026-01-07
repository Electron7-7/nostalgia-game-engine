#include "actor2d.hpp"
#include "managers/theatre_manager.hpp"
#include "thing/thing_factory.hpp"
#include "theatre/parser/thing_data.hpp"

void Actor2D::SetVariables(Farg<ThingData> data)
{
    Thinker::SetVariables(data);
    SetTransformVariables(data);

    data.GetVariable(mDebugMeshInstanceID, "DebugMeshInstance");
    data.GetVariable(mVisible, "Visible");
    data.GetVariable(mWireframe, "MakeWireframe");
    data.GetVariable(mWireframe, "Wireframe");
    data.GetVariable(mDebugHighlight, "DebugHighlight");
}

Shared<ThingData> Actor2D::GetVariables() const
{
    auto data{Thinker::GetVariables()};
    GetTransformVariables(data);

    data->AddVariable(mDebugMeshInstanceID, "DebugMeshInstance");
    data->AddVariable(mVisible, "Visible");
    data->AddVariable(mWireframe, "Wireframe");
    data->AddVariable(mDebugHighlight, "DebugHighlight");

    return data;
}

void Actor2D::Ready()
{ Thinker::Ready(); }

void Actor2D::Tick()
{}

ID Actor2D::DebugMeshInstance() const
{ return mDebugMeshInstanceID; }

void Actor2D::DebugMeshInstance(ID inID)
{ mDebugMeshInstanceID = inID; }

bool Actor2D::Visible() const
{ return mVisible; }

void Actor2D::Visible(bool isVisible)
{ mVisible = isVisible; }

bool Actor2D::Wireframe() const
{ return mWireframe; }

void Actor2D::Wireframe(bool isWireframe)
{ mWireframe = isWireframe; }
