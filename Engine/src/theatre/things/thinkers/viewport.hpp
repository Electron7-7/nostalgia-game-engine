#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "theatre/things/thinkers/thinker.hpp"
#include "math/containers.hpp"
#include "rendering/frame_buffer.hpp"
#include <set>

class Viewport : public Thinker
{
public:
    using cameras = std::set<ID>;

    virtual void Ready() override;
    virtual void Shutdown() override;
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    Farg<Shared<FrameBuffer>> Framebuffer() const;
    void SetFramebuffer(Shared<FrameBuffer>);

    ID CurrentCamera() const;
    Error SetCurrentCamera(ID);

    Farg<cameras> CameraIDs() const;
    bool IsCurrentCamera(ID);
    void AddCamera(ID);
    void EraseCamera(ID);

    Size2D Size() const;
    void SetSize(Farg<Size2D>);

protected:
    Shared<FrameBuffer> mFramebuffer{nullptr};
    Size2D mSize{512, 512};
    cameras mCameraIDs{};
    ID mCurrentCamera{};
};

#endif // VIEWPORT_H
