#include "./visual_3d.hpp"
#include "theatre/theatre.hpp"

using namespace TheatreFile;

void Visual3D::Ready()
{
    Super::Ready();
    auto ancestors{Theatre::Current()->GetAllParents(uid())};
    for(ID parent : ancestors)
    {
        if(Theatre::Current()->DerivedFrom(parent, ThingType::Viewport))
            { mViewportID = parent; break; }
    }
}

void Visual3D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    if(int layers; data.get_variable(layers, "VisualLayers", "RenderLayers", "Layers") == OK)
        { mVisualLayers.set(layers); }

    data.get_variable(mDebugHighlight, "DebugHighlight");
}

Shared<ThingData> Visual3D::GetVariables() const
{
    auto data{Super::GetVariables()};

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
    Super::OnAncestorRemoved(inAncestor);
    if(Theatre::Current()->DerivedFrom(inAncestor.uid, ThingType::Viewport))
        { mViewportID = UID::o_RootViewport; }
}

void Visual3D::OnAncestorAdded(Relative inAncestor)
{
    Super::OnAncestorAdded(inAncestor);
    if(Theatre::Current()->DerivedFrom(inAncestor.uid, ThingType::Viewport))
        { mViewportID = inAncestor.uid; }
}
