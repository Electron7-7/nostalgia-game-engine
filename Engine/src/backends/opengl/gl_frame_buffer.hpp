#ifdef FWD_DCL
class OpenGLFrameBuffer;
#elif !defined GL_FRAME_BUFFER_H
#define GL_FRAME_BUFFER_H

#include "rendering/frame_buffer.hpp"

class OpenGLFrameBuffer : public FrameBuffer
{
public:
    ~OpenGLFrameBuffer() noexcept;

    uint Generate() final;
    void Bind() const final;
    void Unbind() const final;
    uint GetID() const final;
    Error SetOutputTexture(Shared<TextureBuffer> inTexture, ID inID) final;
    ID GetOutputTextureID() const final;
    Farg<Scale2D> GetScale() const final;
    // Error SetOutputRenderBuffer(ID inID) final;
    // ID GetOutputRenderBufferID() const final;

private:
    uint mFrameBufferID{0};
    uint mTextureID{0};
    ID mTextureBufferID{};
    Scale2D mTextureScale{};
    // ID mRenderBufferID{};
};

#endif // GL_FRAME_BUFFER_H
