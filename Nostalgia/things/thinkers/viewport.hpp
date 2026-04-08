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

    Farg<Shared<FrameBuffer>> Framebuffer() const;
    void SetFramebuffer(Shared<FrameBuffer>);

    ID CurrentCamera3D();
    ID CurrentCamera2D();
    void UpdateCurrentCameras();

    Size2D Size() const;
    void SetSize(Farg<Size2D>);

protected:
    Shared<FrameBuffer> mFramebuffer{FrameBuffer::Create()};
    Size2D mSize{512, 512};
    ID mCurrentCamera3D{},
        mCurrentCamera2D{};

    virtual void OnDescendantRemoved(Relative) override;
    virtual void OnDescendantAdded(Relative) override;

    friend class Theatre;
    void SetCurrentCamera3D(ID = ID::Invalid);
    void SetCurrentCamera2D(ID = ID::Invalid);
};

#endif // VIEWPORT_H
