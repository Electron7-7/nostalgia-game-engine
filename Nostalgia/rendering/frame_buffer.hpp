#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <Nostalgia/fwd/rendering.hpp>
#include <Nostalgia/settings/graphics.hpp>

class FrameBuffer
{
public:
    virtual ~FrameBuffer() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual uint ID() const = 0;
    virtual uint RenderBufferID() const = 0;
    virtual uint TextureID() const = 0;
    virtual Error Status() const = 0;
    virtual Shared<TextureBuffer> Texture() const = 0;

    virtual void ClearColor(Farg<ColorRGBA> = Settings::Graphics::ClearColor) const = 0;
    virtual void ClearDepth(float inDepth = 0.0f) const = 0;

    static Shared<FrameBuffer> Create();
    static Shared<FrameBuffer> Create(Farg<Size2D> inSize);
};

#endif // FRAME_BUFFER_H
