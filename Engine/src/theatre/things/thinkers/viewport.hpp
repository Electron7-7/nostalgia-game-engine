#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "theatre/things/thinkers/thinker.hpp"
#include "rendering/frame_buffer.hpp"

class Viewport : public Thinker
{
public:
    virtual void Ready() override;
    virtual void Shutdown() override;
    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

    Farg<Shared<FrameBuffer>> Framebuffer() const;
    void SetFramebuffer(Shared<FrameBuffer>);

    ID CurrentCamera();
    Error SetCurrentCamera(ID);

    Size2D Size() const;
    void SetSize(Farg<Size2D>);

protected:
    Shared<FrameBuffer> mFramebuffer{nullptr};
    Size2D mSize{512, 512};
    ID mCurrentCamera{};
};

#endif // VIEWPORT_H
