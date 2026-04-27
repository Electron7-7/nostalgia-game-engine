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

    virtual Shared<TextureBuffer> Buffer() const { return mBuffer; }

protected:
    Shared<TextureBuffer> mBuffer{TextureBuffer::CreateDummy()};
    RMutex mBufferMutex{};
};

#endif // TEXTURE_H
