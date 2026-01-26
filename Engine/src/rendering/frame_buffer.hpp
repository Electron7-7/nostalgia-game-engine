#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "fwd/rendering.hpp"

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

    // virtual void ClearColor(std::array<float,4> inColor) const = 0;
    // virtual void ClearDepth(float inDepth) const = 0;

    static Shared<FrameBuffer> Create();
    static Shared<FrameBuffer> Create(Farg<Size2D> inSize);
};

#endif // FRAME_BUFFER_H
