#include "./viewport.hpp"
#include "application/application.hpp"
#include "theatre/theatre.hpp"
#include "theatre/thing_factory.hpp"
#include "theatre/things/thinkers/3d/camera_3d.hpp"
#include "theatre/things/thinkers/2d/camera_2d.hpp"

using namespace TheatreFile;

void Viewport::Ready()
{
    if(mUID != UID::o_RootViewport)
        { mFramebuffer = FrameBuffer::Create(mSize); }
    else
        { mFramebuffer = FrameBuffer::Create(); }

    if(mCurrentCamera3D.invalid())
        { SetCurrentCamera3D(); }
    if(mCurrentCamera2D.invalid())
        { SetCurrentCamera2D(); }
}

void Viewport::SetVariables(Farg<ThingData> data)
{
    Thinker::SetVariables(data);

    data.get_variable(mCurrentCamera3D, "CurrentCamera3D");
    data.get_variable(mCurrentCamera2D, "CurrentCamera2D");
    if(glm::vec2 size{}; data.get_variable(size, "ViewportSize", "ContentSize") == OK)
        { mSize = size; }
}

Shared<ThingData> Viewport::GetVariables() const
{
    auto data{Thinker::GetVariables()};

    if(not mCurrentCamera3D.invalid())
        { data->set_variable(mCurrentCamera3D, "CurrentCamera3D"); }
    if(not mCurrentCamera2D.invalid())
        { data->set_variable(mCurrentCamera2D, "CurrentCamera2D"); }

    data->set_variable(glm::vec2{mSize}, "ViewportSize");

    return data;
}

ID Viewport::CurrentCamera3D()
{ return mCurrentCamera3D; }

ID Viewport::CurrentCamera2D()
{ return mCurrentCamera2D; }

Error Viewport::SetCurrentCamera3D(ID inID)
{
    IdSet_t descendants{inID};
    if(inID.invalid())
    {
        if(mUID == UID::o_RootViewport)
            { descendants = my_theatre()->Get3DCameras(); }
        else
            { descendants = my_theatre()->GetAllChildren(mUID); }
        for(ID descendant : descendants)
        {
            if(mCurrentCamera3D != descendant
                and my_theatre()->GetThinker<Camera3D>(descendant)->ViewportID() == mUID)
            {
                mCurrentCamera3D = descendant;
                return OK;
            }
        }
        mCurrentCamera3D = ID::Invalid;
    }
    else if(my_theatre()->GetThinker<Camera3D>(inID)->ViewportID() == mUID)
    {
        mCurrentCamera3D = inID;
        return OK;
    }

    return ERR_NOT_FOUND;
}

Error Viewport::SetCurrentCamera2D(ID inID)
{
    IdSet_t descendants{inID};
    if(inID.invalid())
    {
        if(mUID == UID::o_RootViewport)
            { descendants = my_theatre()->Get2DCameras(); }
        else
            { descendants = my_theatre()->GetAllChildren(mUID); }
        for(ID descendant : descendants)
        {
            if(mCurrentCamera2D != descendant
                and my_theatre()->GetThinker<Camera2D>(descendant)->ViewportID() == mUID)
            {
                mCurrentCamera2D = descendant;
                return OK;
            }
        }
        mCurrentCamera2D = ID::Invalid;
    }
    else if(my_theatre()->GetThinker<Camera2D>(inID)->ViewportID() == mUID)
    {
        mCurrentCamera2D = inID;
        return OK;
    }
    return ERR_NOT_FOUND;
}

Size2D Viewport::Size() const
{
    return (mUID == UID::o_RootViewport)
        ? MainWindow()->GetScale()
        : mSize;
}

Farg<Shared<FrameBuffer>> Viewport::Framebuffer() const
{ return mFramebuffer; }

void Viewport::SetSize(Farg<Size2D> inSize)
{
    if(mSize == inSize)
        { return; }
    mFramebuffer = FrameBuffer::Create(inSize);
    mSize = inSize;
}

void Viewport::SetFramebuffer(Shared<FrameBuffer> inFramebuffer)
{
    if(mUID == UID::o_RootViewport)
        { return; }
    mFramebuffer = inFramebuffer;
}

void Viewport::OnDescendantRemoved(Relative inRelative)
{
    if(inRelative.uid == mCurrentCamera3D)
        { SetCurrentCamera3D(); }
    else if(inRelative.uid == mCurrentCamera2D)
        { SetCurrentCamera2D(); }
}

void Viewport::OnDescendantAdded(Relative inRelative)
{
    if(mCurrentCamera3D.invalid()
        and ThingFactory::IsDerivedFrom(inRelative.type, ThingType::Camera3D))
            { mCurrentCamera3D = inRelative.uid; }
    else if(mCurrentCamera2D.invalid()
        and ThingFactory::IsDerivedFrom(inRelative.type, ThingType::Camera2D))
            { mCurrentCamera2D = inRelative.uid; }
}
