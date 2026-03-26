#include "./camera_2d.hpp"
#include "../viewport.hpp"
#include "settings/graphics.hpp"
#include "theatre/theatre.hpp"
#include "settings/engine.hpp"

using namespace TheatreFile;

void Camera2D::Ready()
{
    Super::Ready();
    auto parent{Theatre::Current()->GetParent(uid())};

    if(mViewportID == UID::o_RootViewport and not parent.invalid())
    {
        if(Theatre::Current()->DerivedFrom(parent, ThingType::Viewport))
            { mViewportID = parent; }
        else
        {
            auto ancestors{Theatre::Current()->GetAllParents(uid())};
            for(ID parent : ancestors)
            {
                if(parent != UID::o_RootViewport and Theatre::Current()->DerivedFrom(parent, ThingType::Viewport))
                    { mViewportID = parent; break; }
            }
        }
    }

    if(auto my_viewport{Theatre::Current()->GetThinker<Viewport>(mViewportID)};
        mInitCurrent and my_viewport->CurrentCamera2D().invalid())
            { my_viewport->SetCurrentCamera2D(uid()); }
}

void Camera2D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    if(int bitmask; data.get_variable(bitmask, "RenderLayersMask", "LayersMask") == OK)
        { mLayersMask.set(bitmask); }
    data.get_variable(mZoom, "Zoom");
    data.get_variable(mInitCurrent, "Current", "CurrentCamera", "IsCurrent");

    mVisible = mVisible and Settings::Engine::IsEditorHint;
}

Shared<ThingData> Camera2D::GetVariables() const
{
    Shared<ThingData> data{Super::GetVariables()};

    data->set_variable(mLayersMask.get(), "RenderLayersMask");
    data->set_variable(mZoom, "Zoom");
    data->set_variable(mInitCurrent, "Current");
    if(!mViewportID.invalid())
        { data->set_variable(mViewportID, "Parent"); }

    return data;
}

ID Camera2D::ViewportID() const
{ return mViewportID; }

bool Camera2D::Current() const
{ return Theatre::Current()->GetThinker<Viewport>(mViewportID)->CurrentCamera2D() == uid(); }

Error Camera2D::SetCurrent(bool isCurrent)
{
    if(isCurrent == Current()) { return OK; }
    return Theatre::Current()
        ->GetThinker<Viewport>(mViewportID)
            ->SetCurrentCamera2D((isCurrent) ? uid() : ID::Invalid);
}

BitMask Camera2D::LayersMask() const
{ return mLayersMask; }

void Camera2D::SetLayersMask(BitMask inLayersMask)
{ mLayersMask = inLayersMask; }

void Camera2D::OnAncestorRemoved(Relative inAncestor)
{
    Super::OnAncestorRemoved(inAncestor);
    if(Theatre::Current()->DerivedFrom(inAncestor.uid, ThingType::Viewport))
        { mViewportID = UID::o_RootViewport; }
}

void Camera2D::OnAncestorAdded(Relative inAncestor)
{
    Super::OnAncestorAdded(inAncestor);
    if(Theatre::Current()->DerivedFrom(inAncestor.uid, ThingType::Viewport))
        { mViewportID = inAncestor.uid; }
}

glm::mat4 Camera2D::ViewMatrix() const
{
    glm::vec2 global_position{(uid().invalid())
        ? glm::vec2{0.0f}
        : GlobalPosition()};
    return glm::lookAt(glm::vec3{global_position, 0.0f},
        {global_position, 1.0f},
        {0.0f, -1.0f, 0.0f});
}

glm::mat4 Camera2D::ProjectionMatrix() const
{
    Scale2D upper{}, lower{},
        viewport_size{(mViewportID == UID::o_RootViewport)
            ? Theatre::Current()->GetRootViewport()->Size()
            : Theatre::Current()->GetThinker<Viewport>(mViewportID)->Size()};
    if(mViewportID == UID::o_RootViewport
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
        upper.y(),
        lower.y(),
        -1.0f,
        1.0f);
}

Farg<glm::vec2> Camera2D::Zoom() const
{ return mZoom; }

void Camera2D::SetZoom(Farg<glm::vec2> inZoom)
{ mZoom = inZoom; }

void Camera2D::SetZoom(float inUniformZoom)
{ mZoom = glm::vec2{inUniformZoom}; }
