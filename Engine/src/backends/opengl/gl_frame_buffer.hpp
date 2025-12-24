#ifdef FWD_DCL
class OpenGLFrameBuffer;
#elif !defined GL_FRAME_BUFFER_H
#define GL_FRAME_BUFFER_H

#include "rendering/frame_buffer.hpp"

class OpenGLFrameBuffer : public FrameBuffer
{
public:
    OpenGLFrameBuffer() noexcept;
    OpenGLFrameBuffer(Farg<Scale2D>) noexcept;
    ~OpenGLFrameBuffer() noexcept;

    void Bind() const final;
    void Unbind() const final;
    uint ID() const final;

    Error Status() const final;

    uint RenderBufferID() const final;
    uint TextureID() const final;
    Scale2D TextureSize() const final;
    Shared<TextureBuffer> Texture() const final;

private:
    uint mBufferID{0};
    uint mRenderBufferID{0};
    Shared<TextureBuffer> mTextureBuffer{nullptr};
    Error mStatus{FAILED};
};

#endif // GL_FRAME_BUFFER_H
