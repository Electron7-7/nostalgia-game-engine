#ifndef TEXTURE_H
#define TEXTURE_H

#include "fwd/rendering.hpp"
#include "core/mutex.hpp"
#include "theatre/things/resources/resource.hpp"
#include "rendering/texture_buffer.hpp"

class Texture : public Resource
{
public:
    virtual void Ready() override;
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

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
