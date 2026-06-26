#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <Nostalgia/things/thinkers/3d/camera_3d.hpp>
#include <Nostalgia/things/thinkers/2d/camera_2d.hpp>
#include <Nostalgia/things/resources/image.hpp>
#include <Nostalgia/rendering/frame_buffer.hpp>
#include <Nostalgia/rendering/render_buffer.hpp>
#include <Nostalgia/rendering/texture_buffer.hpp>

class Viewport : public Thinker
{
public:
	THING_CLASS(Viewport, Thinker)
    READY_OVERRIDE
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    typedef void (*OnSizeChangedCallback_t)(Farg<Size2D<int>>);

    virtual void Attach() const;
    virtual void Detach() const;
    virtual Shared<Image> GetImage() const;

    Shared<TextureBuffer> GetTextureBuffer();
    uint GetTextureBufferID() const;
    Shared<Camera3D> CurrentCamera3D();
    Shared<Camera2D> CurrentCamera2D();
    void SetCurrentCamera3D(Shared<Thing> = nullptr);
    void SetCurrentCamera2D(Shared<Thing> = nullptr);
    void UpdateCurrentCameras();

    Farg<Size2D<int>> Size() const;
    void SetSize(Farg<Size2D<int>>);

protected:
    mutable RMutex mFramebufferMutex{}; // evil mutable
    Shared<FrameBuffer>   mFramebuffer{FrameBuffer::CreateDummy()};
    Shared<RenderBuffer>  mRenderbuffer{RenderBuffer::CreateDummy()};
    Shared<TextureBuffer> mTexturebuffer{TextureBuffer::CreateDummy()};
    Size2D<int> mSize{512, 512};
    Shared<Camera3D> m_pCurrentCamera3D{Camera3D::Invalid()};
    Shared<Camera2D> m_pCurrentCamera2D{Camera2D::Invalid()};

    virtual void OnDescendantRemoved(Relative) override;
    virtual void OnDescendantAdded(Relative) override;
};

#endif // VIEWPORT_H
