#include "visual_3d.hpp"
#include "core/uid.hpp"
#include "theatre/parser/thing_data.hpp"

void Visual3D::SetVariables(Farg<ThingData> data)
{
    Actor3D::SetVariables(data);

    int render_layers{mVisualLayers.get()};

    if(data.GetVariable(render_layers, "VisualLayers", "RenderLayers", "Layers"))
        { mVisualLayers.set(render_layers); }
}

Shared<ThingData> Visual3D::GetVariables() const
{
    auto data{Actor3D::GetVariables()};

    data->AddVariable(mVisualLayers.get(), "VisualLayers");

    return data;
}

BitMask Visual3D::Layers() const
{ return mVisualLayers; }

void Visual3D::SetLayers(BitMask inVisualLayers)
{ mVisualLayers = inVisualLayers; }

IdSet_t Visual3D::Viewports() const
{
    IdSet_t output{mViewportIDs};
    output.insert(UID::a_Global3DViewport);
    return output;
}

void Visual3D::Viewports(IdSet_arg inIDs)
{ mViewportIDs = inIDs; }

Error Visual3D::AddViewport(ID inID)
{
    if(inID.invalid())
        { return ERR_INVALID_ID; }
    return (mViewportIDs.insert(inID).second)
        ? OK
        : ERR_ALREADY_EXISTS;
}

Error Visual3D::RemoveViewport(ID inID)
{
    if(inID.invalid())
        { return ERR_INVALID_ID; }
    return (mViewportIDs.erase(inID))
        ? OK
        : ERR_NOT_FOUND;
}

void Visual3D::ClearViewports()
{ mViewportIDs.clear(); }

bool Visual3D::IsUsingViewport(ID inID) const
{ return inID == UID::a_Global3DViewport or mViewportIDs.contains(inID); }
