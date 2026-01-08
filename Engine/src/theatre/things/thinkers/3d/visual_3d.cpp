#include "visual_3d.hpp"
#include "theatre/parser/thing_data.hpp"

void Visual3D::SetVariables(Farg<ThingData> data)
{
    Actor3D::SetVariables(data);

    int render_layers{mLayers.get()};

    if(data.GetVariable(render_layers, "RenderLayers", "Layers"))
        { mLayers.set(render_layers); }
}

Shared<ThingData> Visual3D::GetVariables() const
{
    auto data{Actor3D::GetVariables()};

    data->AddVariable(mLayers.get(), "RenderLayers");

    return data;
}

RenderLayers Visual3D::Layers() const
{ return mLayers; }

void Visual3D::SetLayers(RenderLayers inLayers)
{ mLayers = inLayers; }

