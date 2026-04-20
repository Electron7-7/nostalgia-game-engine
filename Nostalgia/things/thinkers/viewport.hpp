#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <Nostalgia/things/thinkers/thinker.hpp>
#include <Nostalgia/rendering/frame_buffer.hpp>

class Viewport : public Thinker
{
public:
	SET_SUPER(Thinker)
	SET_TYPEID(ThingType::Viewport)
    READY_OVERRIDE
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual void Clear();

    Farg<Shared<FrameBuffer>> Framebuffer() const;

    ID CurrentCamera3D();
    ID CurrentCamera2D();
    void SetCurrentCamera3D(ID = ID::Invalid);
    void SetCurrentCamera2D(ID = ID::Invalid);
    void UpdateCurrentCameras();

    Size2D Size() const;
    void SetSize(Farg<Size2D>);

protected:
    mutable RMutex mFramebufferMutex{}; // evil mutable
    bool mFramebufferClear{false};
    Shared<FrameBuffer> mFramebuffer{FrameBuffer::Create()};
    Size2D mSize{512, 512};
    ID mCurrentCamera3D{},
        mCurrentCamera2D{};

    virtual void OnDescendantRemoved(Relative) override;
    virtual void OnDescendantAdded(Relative) override;
};

#endif // VIEWPORT_H
