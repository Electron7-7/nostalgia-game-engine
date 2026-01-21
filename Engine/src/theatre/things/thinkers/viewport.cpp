#include "viewport.hpp"
#include "application/application.hpp"
#include "core/uid.hpp"
#include "theatre/theatre.hpp"
#include "theatre/things/thinkers/3d/camera_3d.hpp"
#include "theatre/things/thinkers/2d/camera_2d.hpp"
#include "theatre/parser.hpp"

using namespace TheatreFile;

void Viewport::Ready()
{
    if(mUID == UID::a_Global3DViewport or mUID == UID::a_Global2DViewport)
        { mFramebuffer = FrameBuffer::Create(); }
    else
        { mFramebuffer = FrameBuffer::Create(mSize); }
}

void Viewport::Shutdown()
{}

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

ID Viewport::CurrentCamera()
{ return mCurrentCamera; }

static bool s_IsValidCamera(Theatre* inTheatre, ID inCameraID, ID inViewportID)
{
    if(inTheatre->DerivedFrom(inCameraID, ThingType::Camera3D))
        { return inTheatre->GetThinker<Camera3D>(inCameraID)->ViewportID() == inViewportID; }
    else if(inTheatre->DerivedFrom(inCameraID, ThingType::Camera2D))
        { return inTheatre->GetThinker<Camera2D>(inCameraID)->ViewportID() == inViewportID; }
    return false;
}

Error Viewport::SetCurrentCamera(ID inID)
{
    IdSet_t descendants{inID};
    if(inID.invalid())
    {
        descendants = (mUID == UID::a_Global3DViewport or mUID == UID::a_Global2DViewport)
            ? m_pRootTheatre->GetCameras()
            : m_pRootTheatre->GetAllChildren(mUID);
    }

    for(ID descendant : descendants)
    {
        if(s_IsValidCamera(m_pRootTheatre, descendant, mUID)
            and mCurrentCamera != descendant)
        {
            mCurrentCamera = descendant;
            return OK;
        }
    }
    return ERR_NOT_FOUND;
}

Size2D Viewport::Size() const
{
    return (mUID == UID::a_Global3DViewport)
        ? MainWindow()->GetScale()
        : mSize;
}

Farg<Shared<FrameBuffer>> Viewport::Framebuffer() const
{ return mFramebuffer; }

void Viewport::SetSize(Farg<Size2D> inSize)
{
    if(mSize == inSize)
        { return; }
    else if(mUID != UID::a_Global3DViewport)
        { mFramebuffer = FrameBuffer::Create(inSize); }
    mSize = inSize;
}

void Viewport::SetFramebuffer(Shared<FrameBuffer> inFramebuffer)
{
    if(mUID == UID::a_Global3DViewport)
        { return; }
    mFramebuffer = inFramebuffer;
}
