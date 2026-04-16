#include "./camera_2d.hpp"
#include "../viewport.hpp"
#include "application/application.hpp"
#include "things/thing_data.hpp"
#include "settings/graphics.hpp"
#include "theatre/theatre.hpp"
#include "settings/engine.hpp"

using namespace TheatreFile;

void Camera2D::Ready()
{
    Super::Ready();
    auto parent{Theatre::Current()->GetParent(uid())};

    if(mViewportID.invalid())
    {
        if(not parent.invalid())
        {
            if(Theatre::Current()->DerivedFrom(parent, ThingType::Viewport))
                { mViewportID = parent; }
            else
            {
                auto ancestors{Theatre::Current()->GetAllParents(uid())};
                for(ID parent : ancestors)
                {
                    if(Theatre::Current()->DerivedFrom(parent, ThingType::Viewport))
                        { mViewportID = parent; break; }
                    else if(Theatre::Current()->DerivedFrom(parent, ThingType::NostalgiaPlayer))
                        { mInitCurrent = true; }
                }
            }
        }
    }

    if(mInitCurrent)
        { Theatre::Current()->SetCurrentCamera(uid(), mViewportID); }
}

void Camera2D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mLayersMask, "RenderLayersMask", "LayersMask", "RenderLayers");
    BitMask::StatusArray _render_layers{mLayersMask.status()};
    for(uint i{0}; i < BitMask::max; ++i)
        { data.get_variable(_render_layers[i], std::format("RenderLayer{}", i+1), std::format("Layer{}", i+1)); }
    mLayersMask.set(_render_layers);

    if(glm::vec2 _zoom{mZoom}; data.get_variable(_zoom, "Zoom") == OK)
        { SetZoom(_zoom); } // Make sure mZoom never contains a 0
    data.get_variable(mInitCurrent, "Current", "CurrentCamera", "IsCurrent");

    mVisible = mVisible and Settings::Engine::IsEditorHint;
}

Shared<ThingData> Camera2D::GetVariables() const
{
    Shared<ThingData> data{Super::GetVariables()};

    data->set_variable(mLayersMask, "RenderLayersMask");
    data->set_variable(mZoom, "Zoom");
    data->set_variable(mInitCurrent, "Current");

    return data;
}

ID Camera2D::ViewportID() const
{ return mViewportID; }

bool Camera2D::Current() const
{ return Theatre::Current()->GetCurrentCamera2D(mViewportID) == uid(); }

Error Camera2D::SetCurrent(bool isCurrent)
{
    if(isCurrent == Current())
        { return OK; }
    return Theatre::Current()->SetCurrentCamera(uid(), mViewportID);
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
    Size2D viewport_size{(mViewportID.invalid())
        ? MainWindow()->GetScale()
        : Theatre::Current()->GetThinker<Viewport>(mViewportID)->Size()};
    viewport_size[1] *= -1.0f;

    glm::vec3 _position{GlobalPosition(), 0.0f};
    glm::vec3 _center{GlobalPosition() + (viewport_size.glm() / 2.0f), 0.0f};

    auto _view{glm::lookAt(_position,
        {GlobalPosition(), 1.0f},
        {0.0f, -1.0f, 0.0f})};

    _view = glm::translate(_view, _center);
    _view = glm::scale(_view, {mZoom, 1.0f});
    _view = glm::translate(_view, -_center);

    return _view;
}

glm::mat4 Camera2D::ProjectionMatrix() const
{
    Size2D viewport_size{(mViewportID.invalid())
        ? MainWindow()->GetScale()
        : Theatre::Current()->GetThinker<Viewport>(mViewportID)->Size()};
    float left{0.0f}, right{(float)viewport_size[0]}, up{0.0f}, down{(float)viewport_size[1]},
        _aspect_ratio{static_cast<float>(viewport_size.AspectRatio())};
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
            left = right = 0.0f;
            up = viewport_size[0];
            if(_aspect_ratio < 1.0f)
            {
                down /= _aspect_ratio;
                up   /= _aspect_ratio;
            }
        }
    }

    return glm::ortho(
        left,
        right,
        down,
        up,
        -1.0f,
        1.0f);
}

Farg<glm::vec2> Camera2D::Zoom() const
{ return mZoom; }

void Camera2D::SetZoom(Farg<glm::vec2> inZoom)
{
    if(not inZoom[0] or not inZoom[1])
        { print_warning("Zoom values cannot be 0!"); }

    mZoom[0] = (inZoom[0]) ? inZoom[0] : mZoom[0];
    mZoom[1] = (inZoom[1]) ? inZoom[1] : mZoom[1];
}

void Camera2D::SetZoom(float inUniformZoom)
{ SetZoom(glm::vec2{inUniformZoom}); }
