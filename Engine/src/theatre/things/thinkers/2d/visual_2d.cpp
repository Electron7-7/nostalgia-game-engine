#include "visual_2d.hpp"
#include "theatre/parser.hpp"

using namespace TheatreFile;

void Visual2D::SetVariables(Farg<ThingData> data)
{
    Actor2D::SetVariables(data);

    int render_layers{mVisualLayers.get()};

    if(data.get_variable(render_layers, "VisualLayers", "RenderLayers", "Layers") == OK)
        { mVisualLayers.set(render_layers); }
}

Shared<ThingData> Visual2D::GetVariables() const
{
    auto data{Actor2D::GetVariables()};

    data->set_variable(mVisualLayers.get(), "VisualLayers");

    return data;
}

BitMask Visual2D::Layers() const
{ return mVisualLayers; }

void Visual2D::SetLayers(BitMask inVisualLayers)
{ mVisualLayers = inVisualLayers; }

IdSet_t Visual2D::Viewports() const
{
    IdSet_t output{mViewportIDs};
    output.insert(UID::a_Global2DViewport);
    return output;
}

void Visual2D::Viewports(IdSet_arg inIDs)
{ mViewportIDs = inIDs; }

Error Visual2D::AddViewport(ID inID)
{
    if(inID.invalid())
        { return ERR_INVALID_ID; }
    return (mViewportIDs.insert(inID).second)
        ? OK
        : ERR_ALREADY_EXISTS;
}

Error Visual2D::RemoveViewport(ID inID)
{
    if(inID.invalid())
        { return ERR_INVALID_ID; }
    return (mViewportIDs.erase(inID))
        ? OK
        : ERR_NOT_FOUND;
}

void Visual2D::ClearViewports()
{ mViewportIDs.clear(); }

bool Visual2D::IsUsingViewport(ID inID) const
{ return inID == UID::a_Global2DViewport or mViewportIDs.contains(inID); }
