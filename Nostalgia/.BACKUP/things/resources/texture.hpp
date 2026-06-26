#ifndef TEXTURE_H
#define TEXTURE_H

#include <Nostalgia/things/resources/resource.hpp>
#include <Nostalgia/things/resources/image.hpp>
#include <Nostalgia/rendering/texture_buffer.hpp>

class Texture : public Resource
{
public:
	THING_CLASS(Texture, Resource)

    virtual Shared<TextureBuffer> Buffer() const { return mBuffer; }

protected:
    Shared<TextureBuffer> mBuffer{TextureBuffer::CreateDummy()};
    RMutex mBufferMutex{};
};

inline void Texture::InitVariables()
{
    Super::InitVariables();
}

#endif // TEXTURE_H
