#include "camera_2d.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/parser/thing_data.hpp"
#include "theatre/things/thinkers/viewport.hpp"
#include "settings/engine.hpp"

void Camera2D::SetVariables(Farg<ThingData> data)
{
    Actor2D::SetVariables(data);

    data.GetVariable(mZoom, "FOV");
    data.GetVariable(mViewportID, "Viewport", "ViewportID");
    data.GetVariable(mInitCurrent, "Current", "CurrentCamera", "IsCurrent");

    mVisible = mVisible and Settings::Engine::IsEditorHint;
}

Shared<ThingData> Camera2D::GetVariables() const
{
    Shared<ThingData> data{Actor2D::GetVariables()};

    data->AddVariable(mZoom, "FOV");
    data->AddVariable(mViewportID, "Viewport");
    data->AddVariable(mInitCurrent, "Current");

    return data;
}

void Camera2D::Shutdown()
{ g_pTheatreManager->CurrentTheatre()->GetThinker<Viewport>(mViewportID)->EraseCamera(mUID); }

void Camera2D::Ready()
{
    auto viewport{g_pTheatreManager->CurrentTheatre()->GetThinker<Viewport>(mViewportID)};
    viewport->AddCamera(mUID);
    if(mInitCurrent)
        { viewport->SetCurrentCamera(mUID); }
}

bool Camera2D::Current() const
{ return g_pTheatreManager->CurrentTheatre()->GetThinker<Viewport>(mViewportID)->IsCurrentCamera(mUID); }

ID Camera2D::ViewportID() const
{ return mViewportID; }

BitMask Camera2D::LayersMask() const
{ return mLayersMask; }

bool Camera2D::SetCurrent(bool isCurrent)
{
    if(isCurrent == Current())
        { return isCurrent; }
    else if(isCurrent)
        { return g_pTheatreManager->CurrentTheatre()->GetThinker<Viewport>(mViewportID)->SetCurrentCamera(mUID) == OK; }
    g_pTheatreManager->CurrentTheatre()->GetThinker<Viewport>(mViewportID)->SetCurrentCamera(ID::Invalid);
    return false;
}

Error Camera2D::SetViewportID(ID inID)
{
    if(auto viewport{g_pTheatreManager->CurrentTheatre()->GetThinker<Viewport>(inID)})
    {
        mViewportID = inID;
        viewport->AddCamera(mUID);
        return OK;
    }
    return ERR_INVALID_ID;
}

void Camera2D::SetLayersMask(BitMask inLayersMask)
{ mLayersMask = inLayersMask; }
