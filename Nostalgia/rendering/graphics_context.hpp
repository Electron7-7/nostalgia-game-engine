#ifndef GRAPHICS_CONTEXT_H
#define GRAPHICS_CONTEXT_H

class GraphicsContext
{
public:
    virtual ~GraphicsContext() = default;

    virtual Error Init() = 0;

    static Unique<GraphicsContext> CreateDummy();
    static Unique<GraphicsContext> Create(void* inNativeWindowPointer);
};

class DummyGraphicsContext final : public GraphicsContext
{
public:
    DummyGraphicsContext() = default;
    virtual ~DummyGraphicsContext() = default;

    virtual Error Init() { return FAILED; }
};

#endif // GRAPHICS_CONTEXT_H
