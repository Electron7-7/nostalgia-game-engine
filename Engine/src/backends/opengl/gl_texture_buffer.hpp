#ifndef GL_TEXTURE_BUFFER_H
#define GL_TEXTURE_BUFFER_H

#include "rendering/texture_buffer.hpp"

class OpenGLTextureBuffer final : public TextureBuffer
{
public:
#pragma message("TODO: should each side of the cubemap have its own `TextureFormat` and `SamplerState`?")
    OpenGLTextureBuffer(Farg<TextureFormat>, Farg<SamplerState>, Farg<cubemap_images_t>);
    ~OpenGLTextureBuffer();

    Error Status() const final;
    uint ID() const final;
    Farg<TextureFormat> Format() const final;
    Farg<SamplerState> Sampler() const final;

private:
    uint mBufferID{0};
    TextureFormat mFormat{};
    SamplerState  mSampler{};
    Error mStatus{ERR_UNINITIALIZED};
};

#endif // GL_TEXTURE_BUFFER_H
