#ifndef TEXTURE_H
#define TEXTURE_H

#include <Nostalgia/rendering/texture_buffer.hpp>

class Texture : public Resource
{
public:
    virtual void Ready() override;
    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

    Farg<TextureFormat> Format() const;
    void SetFormat(Farg<TextureFormat>);
    Farg<SamplerState> Sampler() const;
    void SetSampler(Farg<SamplerState>);

    virtual Error Import();
    virtual Shared<TextureBuffer> GetBuffer() const;
    virtual Error SetBuffer(Shared<TextureBuffer>);

protected:
    Shared<TextureBuffer> mTextureBuffer{nullptr};
    RMutex mTextureBufferMutex{};
    TextureFormat mFormat{};
    SamplerState  mSampler{SamplerState::JuliansPreferredDefaults};
    // bool mBoundToFramebuffer{false};
};

#endif // TEXTURE_H
