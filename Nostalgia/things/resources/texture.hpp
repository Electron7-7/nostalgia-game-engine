#ifndef TEXTURE_H
#define TEXTURE_H

#include <Nostalgia/things/resources/resource.hpp>
#include <Nostalgia/things/resources/image.hpp>
#include <Nostalgia/rendering/texture_buffer.hpp>

class Texture : public Resource
{
public:
	SET_SUPER(Resource)
	SET_TYPEID(ThingType::Texture)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    virtual Shared<TextureBuffer> Buffer() const { return mBuffer; }

    Farg<SamplerState> Sampler() const;
    void SetSampler(Farg<SamplerState>);

protected:
    Shared<TextureBuffer> mBuffer{TextureBuffer::CreateDummy()};
    RMutex mBufferMutex{};
    SamplerState mSampler{SamplerState::JuliansPreferredDefaults};
};

#endif // TEXTURE_H
