#ifndef RENDER_BUFFER_H
#define RENDER_BUFFER_H

#include <Nostalgia/rendering/common.hpp>

class RenderBuffer
{
public:
    virtual ~RenderBuffer() = default;

    virtual uint ID() = 0;
    virtual void SetStorage(Farg<Size2D<int>> inSize, DataFormat = DATA_FORMAT_DEPTH) = 0;

    static Shared<RenderBuffer> CreateDummy();
    static Shared<RenderBuffer> Create();
    static Shared<RenderBuffer> Create(Farg<Size2D<int>> inSize, DataFormat = DATA_FORMAT_DEPTH);
};

class DummyRenderBuffer final : public RenderBuffer
{
public:
    DummyRenderBuffer() = default;
    virtual ~DummyRenderBuffer() = default;

    uint ID() final { return 0; }
    void SetStorage(Farg<Size2D<int>>, DataFormat = DATA_FORMAT_DEPTH) final {}
};

#endif // RENDER_BUFFER_H
