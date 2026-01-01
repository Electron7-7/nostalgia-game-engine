#ifndef GL_TEXTURE_BUFFER_H
#define GL_TEXTURE_BUFFER_H

#include "rendering/texture_buffer.hpp"

class OpenGLTextureBuffer final : public TextureBuffer
{
public:
    OpenGLTextureBuffer(Farg<TextureFormat>, Farg<SamplerState>, Shared<FileData>);
    ~OpenGLTextureBuffer();

    Error Status() const final;

    uint ID() const final;
    Farg<TextureFormat> Format() const final;
    Farg<SamplerState>  Sampler() const final;

private:
    uint mBufferID{0};
    TextureFormat mFormat{};
    SamplerState  mSampler{};
    Error mStatus{FAILED};
};

#endif // GL_TEXTURE_BUFFER_H
