#include "viewport.hpp"
#include "application/application.hpp"
#include "core/uid.hpp"
#include "managers/theatre_manager.hpp"
#include "theatre/parser.hpp"

using namespace TheatreFile;

void Viewport::Ready()
{
    if(mCameraIDs.contains(mCurrentCamera) and !g_pTheatreManager->CurrentTheatre()->ThingExists(mCurrentCamera))
    {
        mCameraIDs.erase(mCurrentCamera);
        mCurrentCamera = ID::Invalid;
    }
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
    if(data.get_variable(mCurrentCamera, "CurrentCamera", "Camera") == OK)
        { mCameraIDs.insert(mCurrentCamera); }
}

Shared<ThingData> Viewport::GetVariables() const
{
    auto data{Thinker::GetVariables()};

    data->set_variable(glm::vec2{mSize}, "Size");
    if(!mCurrentCamera.invalid())
        { data->set_variable(mCurrentCamera, "CurrentCamera"); }

    return data;
}

bool Viewport::IsCurrentCamera(ID inID)
{ return mCurrentCamera == inID; }

Error Viewport::SetCurrentCamera(ID inID)
{
    if(mCameraIDs.contains(inID))
        { mCurrentCamera = inID; return OK; }
    else if(inID.invalid())
        { mCurrentCamera = (mCameraIDs.empty()) ? inID : *mCameraIDs.begin(); }
    return ERR_NOT_FOUND;
}

ID Viewport::CurrentCamera() const
{ return mCurrentCamera; }

Farg<Viewport::cameras> Viewport::CameraIDs() const
{ return mCameraIDs; }

Size2D Viewport::Size() const
{
    return (mUID == UID::a_Global3DViewport)
        ? MainWindow()->GetScale()
        : mSize;
}

Farg<Shared<FrameBuffer>> Viewport::Framebuffer() const
{ return mFramebuffer; }

void Viewport::AddCamera(ID inID)
{ mCameraIDs.insert(inID); }

void Viewport::EraseCamera(ID inID)
{
    mCameraIDs.erase(inID);
    if(mCurrentCamera == inID)
        { mCurrentCamera = (mCameraIDs.empty()) ? ID::Invalid : *mCameraIDs.begin(); }
}

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
