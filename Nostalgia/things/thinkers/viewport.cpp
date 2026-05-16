#include "./viewport.hpp"
#include "../thing_data.hpp"
#include "theatre/theatre.hpp"
#include "rendering/renderer_api.hpp"

#define LOCK LockGuard<RMutex> _lock{mFramebufferMutex};

using namespace TheatreFile;

void Viewport::InitVariables()
{
    Super::InitVariables();
}

void Viewport::Ready()
{
    Super::Ready();
    if(m_pCurrentCamera2D->invalid() or m_pCurrentCamera3D->invalid())
        { UpdateCurrentCameras(); }
    mFramebuffer  = FrameBuffer::Create();
    mRenderbuffer = RenderBuffer::Create();
    SetSize(mSize); // Update all buffers
}

void Viewport::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(m_pCurrentCamera3D, "CurrentCamera3D");
    data.get_variable(m_pCurrentCamera2D, "CurrentCamera2D");
    if(glm::vec2 size{}; data.get_variable(size, "ViewportSize", "ContentSize") == OK)
        { SetSize(size); }
}

Shared<ThingData> Viewport::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(m_pCurrentCamera3D, "CurrentCamera3D");
    data->set_variable(m_pCurrentCamera2D, "CurrentCamera2D");
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

Shared<Camera3D> Viewport::CurrentCamera3D()
{ return m_pCurrentCamera3D; }

Shared<Camera2D> Viewport::CurrentCamera2D()
{ return m_pCurrentCamera2D; }

void Viewport::SetCurrentCamera3D(Shared<Thing> inCamera)
{
    if(auto _camera{DCast<Camera3D>(inCamera)})
    {
        m_pCurrentCamera3D = _camera;
        _camera->SetViewportID(uid());
        return;
    }
    m_pCurrentCamera3D = ThingFactory::Invalid<Camera3D>();
    UpdateCurrentCameras();
}

void Viewport::SetCurrentCamera2D(Shared<Thing> inCamera)
{
    if(auto _camera{DCast<Camera2D>(inCamera)})
    {
        m_pCurrentCamera2D = _camera;
        _camera->SetViewportID(uid());
        return;
    }
    m_pCurrentCamera2D = ThingFactory::Invalid<Camera2D>();
    UpdateCurrentCameras();
}

void Viewport::UpdateCurrentCameras()
{
    auto descendants{Theatre::Current()->GetAllChildren(uid())};
    for(FAUTO descendant : descendants)
    {
        if(m_pCurrentCamera3D->invalid() and ThingFactory::DerivedFrom(descendant, ThingType::Camera3D))
            { m_pCurrentCamera3D = ThingFactory::GetThing<Camera3D>(descendant); }
        else if(m_pCurrentCamera2D->invalid() and ThingFactory::DerivedFrom(descendant, ThingType::Camera2D))
            { m_pCurrentCamera2D = ThingFactory::GetThing<Camera2D>(descendant); }
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
    if(inRelative.uid == m_pCurrentCamera3D->uid())
        { m_pCurrentCamera3D = ThingFactory::Invalid<Camera3D>(); }
    else if(inRelative.uid == m_pCurrentCamera2D->uid())
        { m_pCurrentCamera2D = ThingFactory::Invalid<Camera2D>(); }
    if(m_pCurrentCamera2D->invalid() or m_pCurrentCamera3D->invalid())
        { UpdateCurrentCameras(); }
}

void Viewport::OnDescendantAdded(Relative inRelative)
{
    if(m_pCurrentCamera3D->invalid()
        and ThingFactory::IsDerivedFrom(inRelative.type, ThingType::Camera3D))
            { m_pCurrentCamera3D = ThingFactory::GetThing<Camera3D>(inRelative.uid); }
    else if(m_pCurrentCamera2D->invalid()
        and ThingFactory::IsDerivedFrom(inRelative.type, ThingType::Camera2D))
            { m_pCurrentCamera2D = ThingFactory::GetThing<Camera2D>(inRelative.uid); }
}
