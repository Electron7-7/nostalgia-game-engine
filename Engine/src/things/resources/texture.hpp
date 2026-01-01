#ifndef TEXTURE_H
#define TEXTURE_H

#include "fwd/rendering.hpp"
#include "rendering/texture_buffer.hpp"
#include "things/resources/resource.hpp"
#include "core/mutex.hpp"

class Texture : public Resource
{
public:
    virtual void Ready() override;
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    virtual Shared<TextureBuffer> GetBuffer() const;
    virtual Error SetBuffer(Shared<TextureBuffer>);

protected:
    Shared<TextureBuffer> mTextureBuffer{nullptr};
    RMutex mTextureBufferMutex{};
    TextureFormat mFormat{};
    SamplerState  mSampler{};
    // bool mBoundToFramebuffer{false};
};

#endif // TEXTURE_H
