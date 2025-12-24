#ifndef GL_TEXTURE_BUFFER_H
#define GL_TEXTURE_BUFFER_H

#include "rendering/texture_buffer.hpp"

class OpenGLTextureBuffer final : public TextureBuffer
{
public:
    OpenGLTextureBuffer(Farg<TextureFormat>, Farg<SamplerState>, const FileData*);
    ~OpenGLTextureBuffer();

    void Bind() final;
    void Bind(uint) final;
    void Unbind() final;

    Error Status() const final;

    uint ID() const final;
    Farg<TextureFormat> Format() const final;
    Farg<SamplerState>  Sampler() const final;

private:
    bool mIsBound{false};
    uint mBufferID{0};
    TextureFormat mFormat{};
    SamplerState  mSampler{};
    Error mStatus{FAILED};
};

#endif // GL_TEXTURE_BUFFER_H
