#include "./visual_3d.hpp"
#include "theatre/theatre.hpp"

using namespace TheatreFile;

void Visual3D::Ready()
{
    Actor3D::Ready();
    auto ancestors{my_theatre()->GetAllParents(mUID)};
    for(ID parent : ancestors)
    {
        if(my_theatre()->DerivedFrom(parent, ThingType::Viewport))
            { mViewportID = parent; break; }
    }
}

void Visual3D::SetVariables(Farg<ThingData> data)
{
    Actor3D::SetVariables(data);

    if(int layers; data.get_variable(layers, "VisualLayers", "RenderLayers", "Layers") == OK)
        { mVisualLayers.set(layers); }

    data.get_variable(mDebugHighlight, "DebugHighlight");
}

Shared<ThingData> Visual3D::GetVariables() const
{
    auto data{Actor3D::GetVariables()};

    data->set_variable(mVisualLayers.get(), "VisualLayers");
    data->set_variable(mDebugHighlight, "DebugHighlight");

    return data;
}

BitMask Visual3D::Layers() const
{ return mVisualLayers; }

void Visual3D::SetLayers(BitMask inVisualLayers)
{ mVisualLayers = inVisualLayers; }

ID Visual3D::Viewport() const
{ return mViewportID; }

void Visual3D::Viewport(ID inID)
{ mViewportID = inID; }

void Visual3D::OnAncestorRemoved(Relative inAncestor)
{
    Actor3D::OnAncestorRemoved(inAncestor);
    if(my_theatre()->DerivedFrom(inAncestor.uid, ThingType::Viewport))
        { mViewportID = UID::o_RootViewport; }
}

void Visual3D::OnAncestorAdded(Relative inAncestor)
{
    Actor3D::OnAncestorAdded(inAncestor);
    if(my_theatre()->DerivedFrom(inAncestor.uid, ThingType::Viewport))
        { mViewportID = inAncestor.uid; }
}
