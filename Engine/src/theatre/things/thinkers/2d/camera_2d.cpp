#include "camera_2d.hpp"
#include "theatre/things/thinkers/viewport.hpp"
#include "theatre/thing_factory.hpp"
#include "theatre/theatre.hpp"
#include "settings/engine.hpp"
#include "theatre/parser.hpp"

using namespace TheatreFile;

void Camera2D::Ready()
{
    if(mInitCurrent)
        { my_theatre()->GetThinker<Viewport>(mViewportID)->SetCurrentCamera2D(mUID); }
}

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

ID Camera2D::ViewportID() const
{ return mViewportID; }

bool Camera2D::Current() const
{ return my_theatre()->GetThinker<Viewport>(mViewportID)->CurrentCamera2D() == mUID; }

Error Camera2D::SetCurrent(bool isCurrent)
{
    if(isCurrent == Current()) { return OK; }
    return my_theatre()
        ->GetThinker<Viewport>(mViewportID)
            ->SetCurrentCamera((isCurrent) ? mUID : ID::Invalid);
}

BitMask Camera2D::LayersMask() const
{ return mLayersMask; }

void Camera2D::SetLayersMask(BitMask inLayersMask)
{ mLayersMask = inLayersMask; }

void Camera2D::OnAncestorRemoved(Relative inAncestor)
{
    Actor2D::OnAncestorRemoved(inAncestor);
    if(ThingFactory::IsDerivedFrom(inAncestor.type, ThingType::Viewport))
        { mViewportID = UID::a_Global2DViewport; }
}

void Camera2D::OnAncestorAdded(Relative inAncestor)
{
    Actor2D::OnAncestorAdded(inAncestor);
    if(ThingFactory::IsDerivedFrom(inAncestor.type, ThingType::Viewport))
        { mViewportID = inAncestor.uid; }
}
