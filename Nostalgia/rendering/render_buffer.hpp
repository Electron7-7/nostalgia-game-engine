#ifndef RENDER_BUFFER_H
#define RENDER_BUFFER_H

#include <Nostalgia/rendering/common.hpp>

class RenderBuffer
{
public:
    virtual ~RenderBuffer() = default;

    virtual uint ID() = 0;
    virtual void SetStorage(Farg<Size2D> inSize, DataFormat = DATA_FORMAT_DEPTH) = 0;

    static Shared<RenderBuffer> Create();
    static Shared<RenderBuffer> Create(Farg<Size2D> inSize, DataFormat = DATA_FORMAT_DEPTH);
};

#endif // RENDER_BUFFER_H
