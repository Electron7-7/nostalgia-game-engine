#ifndef GRAPHICS_CONTEXT_H
#define GRAPHICS_CONTEXT_H

class GraphicsContext
{
public:
    virtual ~GraphicsContext() = default;

    virtual Error Init() = 0;

    static Unique<GraphicsContext> CreateContext(void* inNativeWindowPointer);
};

#endif // GRAPHICS_CONTEXT_H
