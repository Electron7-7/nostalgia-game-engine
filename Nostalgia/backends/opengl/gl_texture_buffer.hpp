#ifndef GL_TEXTURE_BUFFER_H
#define GL_TEXTURE_BUFFER_H

#include "rendering/texture_buffer.hpp"

class OpenGLTextureBuffer final : public TextureBuffer
{
public:
    OpenGLTextureBuffer(TextureType inType);
    ~OpenGLTextureBuffer();

    void GenerateMipMaps() final;
    void SetSamplerState(Farg<SamplerState>) const final;
    void Load(const uchar*, Farg<TextureFormat>, int) final;
    TextureType Type() const final;
    uint ID() const final;
    Shared<Image> GetImage(int = 0) const final;

private:
    uint mBufferID{0};
    bool mImmutableStorageAlreadyBuffered{false};
    TextureType mType{TEXTURE_TYPE_NONE};
};

#endif // GL_TEXTURE_BUFFER_H
