#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <Nostalgia/fwd/rendering.hpp>
#include <Nostalgia/rendering/common.hpp>
#include <Nostalgia/settings/graphics.hpp>

class FrameBuffer
{
public:
    virtual ~FrameBuffer() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual uint ID() const = 0;
    virtual Error Status(bool inPrintErrorMessage = true) const = 0;
    virtual uint AttachedTextureBufferID() const = 0;
    virtual bool HasAttachedTextureBuffer() const = 0;
    virtual uint AttachedRenderBufferID() const = 0;
    virtual bool HasAttachedRenderBuffer() const = 0;
    virtual void AttachTextureBuffer(Shared<TextureBuffer>, BufferAttachment = ATTACHMENT_COLOR) = 0;
    virtual void DetachTextureBuffer(BufferAttachment = ATTACHMENT_COLOR) = 0;
    virtual void AttachRenderBuffer(Shared<RenderBuffer>, BufferAttachment = ATTACHMENT_DEPTH) = 0;
    virtual void DetachRenderBuffer(BufferAttachment = ATTACHMENT_DEPTH) = 0;

    static Shared<FrameBuffer> CreateDummy();
    static Shared<FrameBuffer> Create();
};

class DummyFrameBuffer final : public FrameBuffer
{
public:
    DummyFrameBuffer() = default;
    virtual ~DummyFrameBuffer() = default;

    void Bind() const final {}
    void Unbind() const final {}
    uint ID() const final { return 0; }
    Error Status(bool = false) const final { return FAILED; }
    uint AttachedTextureBufferID() const final { return 0; }
    bool HasAttachedTextureBuffer() const final { return false; }
    uint AttachedRenderBufferID() const final { return 0; }
    bool HasAttachedRenderBuffer() const final { return false; }
    void AttachTextureBuffer(Shared<TextureBuffer>, BufferAttachment = ATTACHMENT_COLOR) final {}
    void DetachTextureBuffer(BufferAttachment = ATTACHMENT_COLOR) final {}
    void AttachRenderBuffer(Shared<RenderBuffer>, BufferAttachment = ATTACHMENT_DEPTH) final {}
    void DetachRenderBuffer(BufferAttachment = ATTACHMENT_DEPTH) final {}
};

#endif // FRAME_BUFFER_H
