#ifndef GL_FRAME_BUFFER_H
#define GL_FRAME_BUFFER_H

#include <Nostalgia/rendering/frame_buffer.hpp>

class OpenGLFrameBuffer : public FrameBuffer
{
public:
    OpenGLFrameBuffer() noexcept;
    OpenGLFrameBuffer(Farg<Size2D>) noexcept;
    ~OpenGLFrameBuffer() noexcept;

    void Bind() const final;
    void Unbind() const final;
    uint ID() const final;
    uint RenderBufferID() const final;
    uint TextureID() const final;
    Error Status() const final;
    Shared<TextureBuffer> Texture() const final;

private:
    uint mBufferID{0};
    uint mRenderBufferID{0};
    Shared<TextureBuffer> mTextureBuffer{nullptr};
    Error mStatus{FAILED};
};

#endif // GL_FRAME_BUFFER_H
