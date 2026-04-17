#include "./visual_2d.hpp"
#include "things/thing_data.hpp"
#include "theatre/theatre.hpp"

using namespace TheatreFile;

void Visual2D::Ready()
{
    Super::Ready();
    auto ancestors{Theatre::Current()->GetAllParents(uid())};
    for(ID parent : ancestors)
    {
        if(Theatre::Current()->DerivedFrom(parent, ThingType::Viewport))
            { mViewportID = parent; break; }
    }
}

void Visual2D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mVisualLayers, "VisualLayers", "RenderLayers", "Layers");
    data.get_variable(mWireframe, "Wireframe", "WireFrame");
}

Shared<ThingData> Visual2D::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mVisualLayers, "VisualLayers");
    data->set_variable(mWireframe, "Wireframe");

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

bool Visual2D::Wireframe() const
{ return mWireframe; }

void Visual2D::SetWireframe(bool inWireframe)
{ mWireframe = inWireframe; }

void Visual2D::OnAncestorRemoved(Relative inAncestor)
{
    Super::OnAncestorRemoved(inAncestor);
    if(Theatre::Current()->DerivedFrom(inAncestor.uid, ThingType::Viewport))
        { mViewportID = UID::o_RootViewport; }
}

void Visual2D::OnAncestorAdded(Relative inAncestor)
{
    Super::OnAncestorAdded(inAncestor);
    if(Theatre::Current()->DerivedFrom(inAncestor.uid, ThingType::Viewport))
        { mViewportID = inAncestor.uid; }
}
