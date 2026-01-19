#include "camera_2d.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/things/thinkers/viewport.hpp"
#include "settings/engine.hpp"
#include "theatre/parser.hpp"

using namespace TheatreFile;

void Camera2D::SetVariables(Farg<ThingData> data)
{
    Actor2D::SetVariables(data);

    data.get_variable(mZoom, "FOV");
    data.get_variable(mViewportID, "Viewport", "ViewportID");
    data.get_variable(mInitCurrent, "Current", "CurrentCamera", "IsCurrent");

    mVisible = mVisible and Settings::Engine::IsEditorHint;
}

Shared<ThingData> Camera2D::GetVariables() const
{
    Shared<ThingData> data{Actor2D::GetVariables()};

    data->set_variable(mZoom, "FOV");
    data->set_variable(mViewportID, "Viewport");
    data->set_variable(mInitCurrent, "Current");

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
