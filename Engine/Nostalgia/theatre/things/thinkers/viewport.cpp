#include "viewport.hpp"
#include "application/application.hpp"
#include "theatre/theatre.hpp"
#include "theatre/things/thinkers/3d/camera_3d.hpp"
#include "theatre/things/thinkers/2d/camera_2d.hpp"

using namespace TheatreFile;

void Viewport::Ready()
{
    if(mUID != UID::a_RootViewport)
        { mFramebuffer = FrameBuffer::Create(mSize); }
    else
        { mFramebuffer = FrameBuffer::Create(); }

    if(mCurrentCamera3D.invalid())
        { SetCurrentCamera3D(); }
    if(mCurrentCamera2D.invalid())
        { SetCurrentCamera2D(); }

    if(mCurrentCamera3D.invalid() and mCurrentCamera2D.invalid())
    {
        print_warning("Could not find a current Camera2D or Camera3D for Viewport [{}, {}]",
            mName, mUID[]);
    }
}

void Viewport::SetVariables(Farg<ThingData> data)
{
    Thinker::SetVariables(data);

    if(glm::vec2 size{}; data.get_variable(size, "Size", "Scale") == OK)
        { mSize = size; }
}

Shared<ThingData> Viewport::GetVariables() const
{
    auto data{Thinker::GetVariables()};

    data->set_variable(glm::vec2{mSize}, "Size");

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
        if(mUID == UID::a_RootViewport)
            { descendants = my_theatre()->Get3DCameras(); }
        else
            { descendants = my_theatre()->GetAllChildren(mUID); }
    }

    for(ID descendant : descendants)
    {
        if(mCurrentCamera3D != descendant
            and my_theatre()->GetThinker<Camera3D>(descendant)->ViewportID() == mUID)
        {
            mCurrentCamera3D = descendant;
            return OK;
        }
    }
    return ERR_NOT_FOUND;
}

Error Viewport::SetCurrentCamera2D(ID inID)
{
    IdSet_t descendants{inID};
    if(inID.invalid())
    {
        if(mUID == UID::a_RootViewport)
            { descendants = my_theatre()->Get2DCameras(); }
        else
            { descendants = my_theatre()->GetAllChildren(mUID); }
    }

    for(ID descendant : descendants)
    {
        if(mCurrentCamera2D != descendant
            and my_theatre()->GetThinker<Camera2D>(descendant)->ViewportID() == mUID)
        {
            mCurrentCamera2D = descendant;
            return OK;
        }
    }
    return ERR_NOT_FOUND;
}

Size2D Viewport::Size() const
{
    return (mUID == UID::a_RootViewport)
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
    if(mUID == UID::a_RootViewport)
        { return; }
    mFramebuffer = inFramebuffer;
}
