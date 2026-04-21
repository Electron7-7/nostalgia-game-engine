#include "./viewport.hpp"
#include "managers/render_manager.hpp"
#include "things/thing_data.hpp"
#include "theatre/theatre.hpp"
#include "things/thing_factory.hpp"
#include "application/application.hpp"

#define LOCK LockGuard<RMutex> _lock{mFramebufferMutex};

using namespace TheatreFile;

void Viewport::Ready()
{
    Super::Ready();
    if(mCurrentCamera2D.invalid() or mCurrentCamera3D.invalid())
        { UpdateCurrentCameras(); }
}

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

    data->set_variable(mCurrentCamera3D, "CurrentCamera3D");
    data->set_variable(mCurrentCamera2D, "CurrentCamera2D");
    data->set_variable(glm::vec2{mSize}, "ViewportSize");

    return data;
}

void Viewport::Attach() const
{
    LOCK
    mFramebuffer->Bind();
    g_pRenderManager->GetAPI()->SetClearColor(Settings::Graphics::ClearColor);
    g_pRenderManager->GetAPI()->Clear();
    g_pRenderManager->GetAPI()->SetViewport({0, 0}, Size());
}

void Viewport::Detach() const
{
    LOCK
    mFramebuffer->Unbind();
}

Farg<Shared<FrameBuffer>> Viewport::Framebuffer() const
{ return mFramebuffer; }

ID Viewport::CurrentCamera3D()
{ return mCurrentCamera3D; }

ID Viewport::CurrentCamera2D()
{ return mCurrentCamera2D; }

void Viewport::SetCurrentCamera3D(ID inUID)
{
    mCurrentCamera3D = inUID;
    if(inUID.invalid())
        { UpdateCurrentCameras(); }
}

void Viewport::SetCurrentCamera2D(ID inUID)
{
    mCurrentCamera2D = inUID;
    if(inUID.invalid())
        { UpdateCurrentCameras(); }
}

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

Size2D Viewport::Size() const
{
    return (Invalid())
        ? MainWindow()->GetScale()
        : mSize;
}

void Viewport::SetSize(Farg<Size2D> inSize)
{
    if(mSize == inSize)
        { return; }
    mFramebuffer = FrameBuffer::Create(inSize);
    mSize = inSize;
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
