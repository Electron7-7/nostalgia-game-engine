#ifndef GL_FRAME_BUFFER_H
#define GL_FRAME_BUFFER_H

#include "rendering/frame_buffer.hpp"
#include "rendering/texture_buffer.hpp"
#include "rendering/render_buffer.hpp"

class OpenGLFrameBuffer : public FrameBuffer
{
public:
    OpenGLFrameBuffer() noexcept;
    ~OpenGLFrameBuffer() noexcept;

    void Bind() const final;
    void Unbind() const final;
    uint ID() const final;
    Error Status(bool inPrintErrorMessage = true) const final;
    uint AttachedTextureBufferID() const final;
    bool HasAttachedTextureBuffer() const final;
    uint AttachedRenderBufferID() const final;
    bool HasAttachedRenderBuffer() const final;
    void AttachTextureBuffer(Shared<TextureBuffer>, BufferAttachment = ATTACHMENT_COLOR) final;
    void DetachTextureBuffer(BufferAttachment = ATTACHMENT_COLOR) final;
    void AttachRenderBuffer(Shared<RenderBuffer>, BufferAttachment = ATTACHMENT_DEPTH) final;
    void DetachRenderBuffer(BufferAttachment = ATTACHMENT_DEPTH) final;

private:
    uint mBufferID{0},
        mAttachedRenderBufferID{0},
        mAttachedTextureBufferID{0};
    bool mHasTextureBuffer{false}, mHasRenderBuffer{false};
};

#endif // GL_FRAME_BUFFER_H
