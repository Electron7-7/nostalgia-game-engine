#ifndef TEXTURE_H
#define TEXTURE_H

#include "rendering/texture_buffer.hpp"

class Texture : public Resource
{
public:
    virtual void Ready() override;
    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

    virtual Error Import();

    virtual Farg<TextureFormat> Format() const;
    virtual void SetFormat(Farg<TextureFormat>);
    virtual Farg<SamplerState> Sampler() const;
    virtual void SetSampler(Farg<SamplerState>);

    virtual Shared<TextureBuffer> GetBuffer() const;
    virtual Error SetBuffer(Shared<TextureBuffer>);

protected:
    cubemap_images_t m_pImages{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
    Shared<TextureBuffer> mTextureBuffer{nullptr};
    RMutex mTextureBufferMutex{};
    TextureFormat mFormat{};
    SamplerState  mSampler{};
    // bool mBoundToFramebuffer{false};
};

#endif // TEXTURE_H
