#include "visual_2d.hpp"
#include "theatre/parser/thing_data.hpp"

void Visual2D::SetVariables(Farg<ThingData> data)
{
    Actor2D::SetVariables(data);

    int render_layers{mVisualLayers.get()};

    if(data.GetVariable(render_layers, "VisualLayers", "RenderLayers", "Layers"))
        { mVisualLayers.set(render_layers); }
}

Shared<ThingData> Visual2D::GetVariables() const
{
    auto data{Actor2D::GetVariables()};

    data->AddVariable(mVisualLayers.get(), "VisualLayers");

    return data;
}

BitMask Visual2D::Layers() const
{ return mVisualLayers; }

void Visual2D::SetLayers(BitMask inVisualLayers)
{ mVisualLayers = inVisualLayers; }
