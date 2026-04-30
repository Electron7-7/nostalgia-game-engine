#include "./visual_3d.hpp"
#include "things/thing_data.hpp"
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

    data.get_variable(mVisualLayers, "VisualLayers", "RenderLayers", "Layers");
}

Shared<ThingData> Visual3D::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mVisualLayers, "VisualLayers");

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
        { mViewportID = ID::Invalid; }
}

void Visual3D::OnAncestorAdded(Relative inAncestor)
{
    Super::OnAncestorAdded(inAncestor);
    if(Theatre::Current()->DerivedFrom(inAncestor.uid, ThingType::Viewport))
        { mViewportID = inAncestor.uid; }
}
