#include "visual_3d.hpp"
#include "theatre/parser.hpp"
#include "theatre/theatre.hpp"
#include "theatre/thing_factory.hpp"

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

    int render_layers{mVisualLayers.get()};

    if(data.get_variable(render_layers, "VisualLayers", "RenderLayers", "Layers") == OK)
        { mVisualLayers.set(render_layers); }

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
    if(ThingFactory::IsDerivedFrom(inAncestor.type, ThingType::Viewport))
        { mViewportID = UID::a_RootViewport; }
}

void Visual3D::OnAncestorAdded(Relative inAncestor)
{
    Actor3D::OnAncestorAdded(inAncestor);
    if(ThingFactory::IsDerivedFrom(inAncestor.type, ThingType::Viewport))
        { mViewportID = inAncestor.uid; }
}
