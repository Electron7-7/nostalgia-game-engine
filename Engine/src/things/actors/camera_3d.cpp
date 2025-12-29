#include "camera_3d.hpp"
#include "theatre/parser/thing_data.hpp"

//////////////
// Camera3D //
//////////////
void Camera3D::SetVariables(Farg<ThingData> data)
{
    Actor::SetVariables(data);

    data.GetVariable(mCurrent, "Current", "CurrentCamera", "IsCurrent");
}

Shared<ThingData> Camera3D::GetVariables() const
{
    Shared<ThingData> data{Actor::GetVariables()};

    data->AddVariable(mCurrent, "Current");

    return data;
}

void Camera3D::SetRenderLayers(Farg<RenderLayers> inLayers)
{ mRenderLayers = inLayers; }

Farg<RenderLayers> Camera3D::GetRenderLayers() const
{ return mRenderLayers; }

void Camera3D::SetCurrent(bool isCurrent)
{ mCurrent = isCurrent; }

bool Camera3D::IsCurrent() const
{ return mCurrent; }

////////////////////
// EditorCamera3D //
////////////////////
void EditorCamera3D::SetVariables(Farg<ThingData> data)
{ Camera3D::SetVariables(data); }

Shared<ThingData> EditorCamera3D::GetVariables() const
{
    Shared<ThingData> data{Camera3D::GetVariables()};
    return data;
}
