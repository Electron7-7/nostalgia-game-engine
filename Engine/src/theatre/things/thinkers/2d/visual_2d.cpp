#include "visual_2d.hpp"
#include "theatre/theatre.hpp"

using namespace TheatreFile;

void Visual2D::Ready()
{
    Actor2D::Ready();
    auto ancestors{my_theatre()->GetAllParents(mUID)};
    for(ID parent : ancestors)
    {
        if(my_theatre()->DerivedFrom(parent, ThingType::Viewport))
            { mViewportID = parent; break; }
    }
}

void Visual2D::SetVariables(Farg<ThingData> data)
{
    Actor2D::SetVariables(data);

    if(int layers; data.get_variable(layers, "VisualLayers", "RenderLayers", "Layers") == OK)
        { mVisualLayers.set(layers); }

    data.get_variable(mDebugHighlight, "DebugHighlight");
}

Shared<ThingData> Visual2D::GetVariables() const
{
    auto data{Actor2D::GetVariables()};

    data->set_variable(mVisualLayers.get(), "VisualLayers");
    data->set_variable(mDebugHighlight, "DebugHighlight");

    return data;
}

BitMask Visual2D::Layers() const
{ return mVisualLayers; }

void Visual2D::SetLayers(BitMask inVisualLayers)
{ mVisualLayers = inVisualLayers; }

ID Visual2D::Viewport() const
{ return mViewportID; }

void Visual2D::Viewport(ID inID)
{ mViewportID = inID; }

void Visual2D::OnAncestorRemoved(Relative inAncestor)
{
    Actor2D::OnAncestorRemoved(inAncestor);
    if(my_theatre()->DerivedFrom(inAncestor.uid, ThingType::Viewport))
        { mViewportID = UID::a_RootViewport; }
}

void Visual2D::OnAncestorAdded(Relative inAncestor)
{
    Actor2D::OnAncestorAdded(inAncestor);
    if(my_theatre()->DerivedFrom(inAncestor.uid, ThingType::Viewport))
        { mViewportID = inAncestor.uid; }
}
