#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <Nostalgia/rendering/frame_buffer.hpp>

class Viewport : public Thinker
{
public:
    SUPER(Thinker)
    READY_OVERRIDE
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    Farg<Shared<FrameBuffer>> Framebuffer() const;
    void SetFramebuffer(Shared<FrameBuffer>);

    ID CurrentCamera3D();
    Error SetCurrentCamera3D(ID = ID::Invalid);

    ID CurrentCamera2D();
    Error SetCurrentCamera2D(ID = ID::Invalid);

    Size2D Size() const;
    void SetSize(Farg<Size2D>);

protected:
    Shared<FrameBuffer> mFramebuffer{nullptr};
    Size2D mSize{512, 512};
    ID mCurrentCamera3D{},
        mCurrentCamera2D{};

    virtual void OnDescendantRemoved(Relative) override;
    virtual void OnDescendantAdded(Relative) override;
};

#endif // VIEWPORT_H
