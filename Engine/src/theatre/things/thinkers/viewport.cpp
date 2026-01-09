#include "viewport.hpp"
#include "application/application.hpp"
#include "theatre/parser/thing_data.hpp"
#include "core/uid.hpp"
#include "managers/theatre_manager.hpp"

void Viewport::Ready()
{
    if(mCameraIDs.contains(mCurrentCamera) and !g_pTheatreManager->ThingExists(mCurrentCamera))
    {
        mCameraIDs.erase(mCurrentCamera);
        mCurrentCamera = ID::Invalid;
    }
    if(mUID <= UID::a_Global3DViewport)
        { mFramebuffer = FrameBuffer::Create(); }
    else
        { mFramebuffer = FrameBuffer::Create(mSize); }
}

void Viewport::Shutdown()
{}

void Viewport::SetVariables(Farg<ThingData> data)
{
    Thinker::SetVariables(data);

    if(glm::vec2 size{}; data.GetVariable(size, "Size", "Scale"))
        { mSize = size; }
    if(data.GetVariable(mCurrentCamera, "CurrentCamera", "Camera"))
        { mCameraIDs.insert(mCurrentCamera); }
}

Shared<ThingData> Viewport::GetVariables() const
{
    auto data{Thinker::GetVariables()};

    data->AddVariable(glm::vec2{mSize}, "Size");
    if(!mCurrentCamera.invalid())
        { data->AddVariable(mCurrentCamera, "CurrentCamera"); }

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
