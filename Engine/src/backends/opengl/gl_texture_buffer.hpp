#ifndef GL_TEXTURE_BUFFER_H
#define GL_TEXTURE_BUFFER_H

#include "rendering/texture_buffer.hpp"

class OpenGLTextureBuffer final : public TextureBuffer
{
public:
    OpenGLTextureBuffer();
    ~OpenGLTextureBuffer();

    virtual void Bind() const final;
    virtual void Unbind() const final;

    void BindUnit(uint) final;
    void UnbindUnit() const final;

    uint GetID() const final;
    Farg<TextureFormat> GetFormat() const final;

    Error GenerateTexture(Farg<TextureFormat>) final;
    Error GenerateTexture(Farg<FileData>, Farg<TextureFormat>) final;

private:
    uint mTextureID{0}, mTextureUnit{0};
    TextureFormat mTextureFormat{};
};

#endif // GL_TEXTURE_BUFFER_H
