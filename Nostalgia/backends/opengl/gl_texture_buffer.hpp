#ifndef GL_TEXTURE_BUFFER_H
#define GL_TEXTURE_BUFFER_H

#include <Nostalgia/rendering/texture_buffer.hpp>

class OpenGLTextureBuffer final : public TextureBuffer
{
public:
    OpenGLTextureBuffer();
    ~OpenGLTextureBuffer();

    Error GenerateMipMaps() final;
    Error SetSamplerState(Farg<SamplerState>) const final;
    Error Load(InputData, Farg<TextureFormat>, int) final;
    TextureType Type() const final;
    Error Status() const final;
    uint ID() const final;

private:
    uint mBufferID{0};
    TextureType mType{TEXTURE_TYPE_NONE};
    Error mStatus{ERR_UNINITIALIZED};
};

#endif // GL_TEXTURE_BUFFER_H
