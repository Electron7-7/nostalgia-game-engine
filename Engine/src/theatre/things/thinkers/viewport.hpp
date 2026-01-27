#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "rendering/frame_buffer.hpp"

class Viewport : public Thinker
{
public:
    virtual void Ready() override;
    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

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
};

#endif // VIEWPORT_H
