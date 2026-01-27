#include "camera_2d.hpp"
#include "../viewport.hpp"
#include "settings/graphics.hpp"
#include "settings/world.hpp"
#include "theatre/theatre.hpp"
#include "settings/engine.hpp"

using namespace TheatreFile;

void Camera2D::Ready()
{
    Actor2D::Ready();
    auto ancestors{my_theatre()->GetAllParents(mUID)};
    for(ID parent : ancestors)
    {
        if(my_theatre()->DerivedFrom(parent, ThingType::Viewport))
            { mViewportID = parent; break; }
    }

    if(mInitCurrent)
        { my_theatre()->GetThinker<Viewport>(mViewportID)->SetCurrentCamera2D(mUID); }
}

void Camera2D::SetVariables(Farg<ThingData> data)
{
    Actor2D::SetVariables(data);

    if(int bitmask; data.get_variable(bitmask, "RenderLayersMask", "LayersMask") == OK)
        { mLayersMask.set(bitmask); }
    data.get_variable(mZoom, "FOV");
    data.get_variable(mViewportID, "Viewport", "ViewportID");
    data.get_variable(mInitCurrent, "Current", "CurrentCamera", "IsCurrent");

    mVisible = mVisible and Settings::Engine::IsEditorHint;
}

Shared<ThingData> Camera2D::GetVariables() const
{
    Shared<ThingData> data{Actor2D::GetVariables()};

    data->set_variable(mLayersMask.get(), "RenderLayersMask");
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
            ->SetCurrentCamera2D((isCurrent) ? mUID : ID::Invalid);
}

BitMask Camera2D::LayersMask() const
{ return mLayersMask; }

void Camera2D::SetLayersMask(BitMask inLayersMask)
{ mLayersMask = inLayersMask; }

void Camera2D::OnAncestorRemoved(Relative inAncestor)
{
    Actor2D::OnAncestorRemoved(inAncestor);
    if(ThingFactory::IsDerivedFrom(inAncestor.type, ThingType::Viewport))
        { mViewportID = UID::a_RootViewport; }
}

void Camera2D::OnAncestorAdded(Relative inAncestor)
{
    Actor2D::OnAncestorAdded(inAncestor);
    if(ThingFactory::IsDerivedFrom(inAncestor.type, ThingType::Viewport))
        { mViewportID = inAncestor.uid; }
}

glm::mat4 Camera2D::ViewMatrix() const
{
    return glm::lookAt(glm::vec3{GlobalPosition(), 0.0f},
        {GlobalPosition(), -1.0f},
        Settings::World::Up());
}

glm::mat4 Camera2D::ProjectionMatrix() const
{
    Scale2D viewport_size{my_theatre()->GetThinker<Viewport>(mViewportID)->Size()},
        upper{},
        lower{};
    if(mViewportID == UID::a_RootViewport
        and Settings::Graphics::Stretch::Mode == Settings::Graphics::StretchMode::Viewport)
    {
        switch(Settings::Graphics::Stretch::Aspect)
        {
        case Settings::Graphics::StretchAspect::Ignore:
        default:
            break;
        case Settings::Graphics::StretchAspect::Keep:
            // https://gamedev.stackexchange.com/a/49698
            auto aspect{viewport_size.AspectRatio()};
            lower[0] = upper[0] = 0.0f;
            lower[1] = viewport_size[0];
            upper[1] = viewport_size[1];
            if(aspect < 1.0f)
            {
                lower[1] /= aspect;
                upper[1] /= aspect;
            }
        }
    }
    else
        { upper = viewport_size; }

    return glm::ortho(lower.x(),
        upper.x(),
        lower.y(),
        upper.y(),
        -1.0f,
        1.0f);
}

Farg<glm::vec2> Camera2D::Zoom() const
{ return mZoom; }

void Camera2D::SetZoom(Farg<glm::vec2> inZoom)
{ mZoom = inZoom; }

void Camera2D::SetZoom(float inUniformZoom)
{ mZoom = glm::vec2{inUniformZoom}; }
