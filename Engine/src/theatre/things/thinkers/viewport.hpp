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

    bool IsCurrentCamera(ID);
    Error CurrentCamera(ID);
    ID CurrentCamera() const;
    Farg<cameras> CameraIDs() const;
    Size2D Size() const;
    Farg<Shared<FrameBuffer>> Framebuffer() const;
    void AddCamera(ID);
    void EraseCamera(ID);
    void Size(Farg<Size2D>);
    void Framebuffer(Shared<FrameBuffer>);

protected:
    Shared<FrameBuffer> mFramebuffer{nullptr};
    Size2D mSize{512, 512};
    cameras mCameraIDs{};
    ID mCurrentCamera{};
};

#endif // VIEWPORT_H
