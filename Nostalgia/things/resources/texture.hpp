#ifndef TEXTURE_H
#define TEXTURE_H

#include <Nostalgia/things/resources/resource.hpp>
#include <Nostalgia/rendering/texture_buffer.hpp>

class Texture : public Resource
{
public:
	SET_SUPER(Resource)
	SET_TYPEID(ThingType::Texture)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual Shared<TextureBuffer> GetBuffer() const;
    virtual Error SetBuffer(Shared<TextureBuffer>);

    Farg<TextureFormat> Format() const;
    Farg<SamplerState> Sampler() const;
    void SetSampler(Farg<SamplerState>);

protected:
    Shared<TextureBuffer> mTextureBuffer{nullptr};
    RMutex mTextureBufferMutex{};
    TextureFormat mFormat{};
    SamplerState  mSampler{SamplerState::JuliansPreferredDefaults};
};

#endif // TEXTURE_H
