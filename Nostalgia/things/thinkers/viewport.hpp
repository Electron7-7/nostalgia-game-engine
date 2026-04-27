#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <Nostalgia/things/thinkers/thinker.hpp>
#include <Nostalgia/things/resources/image.hpp>
#include <Nostalgia/rendering/frame_buffer.hpp>
#include <Nostalgia/rendering/render_buffer.hpp>
#include <Nostalgia/rendering/texture_buffer.hpp>

class Viewport : public Thinker
{
public:
	SET_SUPER(Thinker)
	SET_TYPEID(ThingType::Viewport)
    READY_OVERRIDE
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    typedef void (*OnSizeChangedCallback_t)(Farg<Size2D>);

    virtual void Attach() const;
    virtual void Detach() const;
    virtual Shared<Image> GetImage() const;

    Shared<TextureBuffer> GetTextureBuffer();
    uint GetTextureBufferID() const;
    ID CurrentCamera3D();
    ID CurrentCamera2D();
    void SetCurrentCamera3D(ID = ID::Invalid);
    void SetCurrentCamera2D(ID = ID::Invalid);
    void UpdateCurrentCameras();

    Farg<Size2D> Size() const;
    void SetSize(Farg<Size2D>);

protected:
    mutable RMutex mFramebufferMutex{}; // evil mutable
    Shared<FrameBuffer>   mFramebuffer{FrameBuffer::CreateDummy()};
    Shared<RenderBuffer>  mRenderbuffer{RenderBuffer::CreateDummy()};
    Shared<TextureBuffer> mTexturebuffer{TextureBuffer::CreateDummy()};
    Size2D mSize{512, 512};
    ID mCurrentCamera3D{},
        mCurrentCamera2D{};

    virtual void OnDescendantRemoved(Relative) override;
    virtual void OnDescendantAdded(Relative) override;
};

#endif // VIEWPORT_H
