#include "./viewport.hpp"
#include "rendering/renderer_api.hpp"
#include "things/thing_data.hpp"
#include "theatre/theatre.hpp"
#include "things/thing_factory.hpp"

#define LOCK LockGuard<RMutex> _lock{mFramebufferMutex};

using namespace TheatreFile;

void Viewport::Ready()
{
    Super::Ready();
    if(mCurrentCamera2D.invalid() or mCurrentCamera3D.invalid())
        { UpdateCurrentCameras(); }
    mFramebuffer = FrameBuffer::Create();
    SetSize(mSize); // Update all buffers
}

void Viewport::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mCurrentCamera3D, "CurrentCamera3D");
    data.get_variable(mCurrentCamera2D, "CurrentCamera2D");
    if(glm::vec2 size{}; data.get_variable(size, "ViewportSize", "ContentSize") == OK)
        { SetSize(size); }
}

Shared<ThingData> Viewport::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mCurrentCamera3D, "CurrentCamera3D");
    data->set_variable(mCurrentCamera2D, "CurrentCamera2D");
    data->set_variable(mSize.glm(), "ViewportSize");

    return data;
}

void Viewport::Attach() const
{
    LOCK
    mFramebuffer->Bind();
    RendererAPI::Get()->SetClearColor(Settings::Graphics::ClearColor);
    RendererAPI::Get()->Clear();
    RendererAPI::Get()->SetViewport({0, 0}, Size());
}

void Viewport::Detach() const
{
    LOCK
    mFramebuffer->Unbind();
}

Shared<Image> Viewport::GetImage() const
{ return mTexturebuffer->GetImage(); }

Shared<TextureBuffer> Viewport::GetTextureBuffer()
{ return mTexturebuffer; }

uint Viewport::GetTextureBufferID() const
{ return mTexturebuffer->GetID(); }

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

Farg<Size2D> Viewport::Size() const
{ return mSize; }

void Viewport::SetSize(Farg<Size2D> inSize)
{
    if(inSize[0] == 0 or inSize[1] == 0)
        { print_warning("Cannot set Viewport width/height to 0"); }
    mSize[0] = (inSize[0]) ? inSize[0] : mSize[0];
    mSize[1] = (inSize[1]) ? inSize[1] : mSize[1];

    LOCK
    mTexturebuffer = TextureBuffer::Create({TEXTURE_TYPE_2D, mSize.w(), mSize.h(), DATA_FORMAT_SRGB});
    mTexturebuffer->SetSamplerState({SAMPLER_FILTER_NEAREST, SAMPLER_FILTER_NONE, SAMPLER_FILTER_NEAREST});
    mRenderbuffer->SetStorage(inSize);
    mFramebuffer->AttachRenderBuffer(mRenderbuffer);
    mFramebuffer->AttachTextureBuffer(mTexturebuffer);
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
