#ifndef GL_TEXTURE_BUFFER_H
#define GL_TEXTURE_BUFFER_H

#include "rendering/texture_buffer.hpp"

class OpenGLTextureBuffer final : public TextureBuffer
{
public:
    OpenGLTextureBuffer(Farg<TextureFormat>);
    ~OpenGLTextureBuffer();

    void GenerateMipmaps() final;
    void SetSamplerState(Farg<SamplerState>) final;
    void SetData(Farg<TextureDataFormat>) final;
    uint GetID() final;
    Farg<TextureFormat> GetFormat() final;
    void GetSamplerState(SamplerState&) final;
    Shared<Image> GetImage(int = 0, int = 0) final;

private:
    uint mBufferID{0};
    TextureFormat mFormat{};
};

#endif // GL_TEXTURE_BUFFER_H
