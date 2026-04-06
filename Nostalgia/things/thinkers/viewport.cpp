#include "./viewport.hpp"
#include "things/thing_data.hpp"
#include "theatre/theatre.hpp"
#include "things/thing_factory.hpp"
#include "application/application.hpp"

using namespace TheatreFile;

void Viewport::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mCurrentCamera3D, "CurrentCamera3D");
    data.get_variable(mCurrentCamera2D, "CurrentCamera2D");
    if(glm::vec2 size{}; data.get_variable(size, "ViewportSize", "ContentSize") == OK)
        { mSize = size; }
    mFramebuffer = FrameBuffer::Create(mSize);
}

Shared<ThingData> Viewport::GetVariables() const
{
    auto data{Super::GetVariables()};

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

void Viewport::UpdateCurrentCameras()
{
    auto descendants{Theatre::Current()->GetAllChildren(uid())};
    for(FAUTO descendant : descendants)
    {
        if(mCurrentCamera3D.invalid() and Theatre::Current()->DerivedFrom(descendant, ThingType::Camera3D))
            { mCurrentCamera3D = descendant; }
        else if(mCurrentCamera2D.invalid() and Theatre::Current()->DerivedFrom(descendant, ThingType::Camera2D))
            { mCurrentCamera2D = descendant; }
    }
}

void Viewport::SetCurrentCamera3D(ID inID)
{
    mCurrentCamera3D = inID;
    if(inID.invalid())
        { UpdateCurrentCameras(); }
}

void Viewport::SetCurrentCamera2D(ID inID)
{
    mCurrentCamera2D = inID;
    if(inID.invalid())
        { UpdateCurrentCameras(); }
}

Size2D Viewport::Size() const
{
    return (uid().invalid())
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
    if(uid() == UID::o_RootViewport)
        { return; }
    mFramebuffer = inFramebuffer;
}

void Viewport::OnDescendantRemoved(Relative inRelative)
{
    if(inRelative.uid == mCurrentCamera3D)
        { mCurrentCamera3D = {}; }
    else if(inRelative.uid == mCurrentCamera2D)
        { mCurrentCamera2D = {}; }
    if(mCurrentCamera2D.invalid() or mCurrentCamera3D.invalid())
        { UpdateCurrentCameras(); }
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
