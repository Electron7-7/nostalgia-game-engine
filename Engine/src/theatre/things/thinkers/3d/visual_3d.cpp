#include "visual_3d.hpp"
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

