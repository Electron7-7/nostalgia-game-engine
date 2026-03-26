#ifndef TEXTURE_H
#define TEXTURE_H

#include <Nostalgia/rendering/texture_buffer.hpp>

class Texture : public Resource
{
public:
    static Shared<Texture> CreateFromMemory(const uchar* inData, size_t inSize);

    SUPER(Resource)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    Farg<TextureFormat> Format() const;
    void SetFormat(Farg<TextureFormat>);
    Farg<SamplerState> Sampler() const;
    void SetSampler(Farg<SamplerState>);

    virtual Error Import();
    virtual Shared<TextureBuffer> GetBuffer() const;
    virtual Error SetBuffer(Shared<TextureBuffer>);

protected:
    Shared<FileData> m_pImage{MakeShared<FileData>()};
    Shared<TextureBuffer> mTextureBuffer{nullptr};
    RMutex mTextureBufferMutex{};
    TextureFormat mFormat{};
    SamplerState  mSampler{SamplerState::JuliansPreferredDefaults};
    // bool mBoundToFramebuffer{false};
};

#endif // TEXTURE_H
