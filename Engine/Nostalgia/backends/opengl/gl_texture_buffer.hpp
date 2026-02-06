#ifndef GL_TEXTURE_BUFFER_H
#define GL_TEXTURE_BUFFER_H

#ifdef NOSTALGIA_GCC_PCH
#   include <Nostalgia/Nostalgia.hpp>
#endif

#include <Nostalgia/rendering/texture_buffer.hpp>

class OpenGLTextureBuffer final : public TextureBuffer
{
public:
    OpenGLTextureBuffer();
    ~OpenGLTextureBuffer();

    Error GenerateMipMaps() final;
    Error SetSamplerState(Farg<SamplerState>) const final;
    Error Load(InputData, Farg<TextureFormat>) final;
    TextureType Type() const final;
    Error Status() const final;
    uint ID() const final;

private:
    uint mBufferID{0};
    TextureType mType{TEXTURE_TYPE_NONE};
    Error mStatus{ERR_UNINITIALIZED};
    int mCubemapOffsetIterator{0};
};

#endif // GL_TEXTURE_BUFFER_H
