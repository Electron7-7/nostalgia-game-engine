#ifndef GRAPHICS_CONTEXT_H
#define GRAPHICS_CONTEXT_H

#ifdef NOSTALGIA_GCC_PCH
#   include <Nostalgia/Nostalgia.hpp>
#endif

class IGraphicsContext
{
public:
    virtual ~IGraphicsContext() = default;

    virtual Error Init() = 0;
    virtual void SwapBuffers() = 0;

    static Unique<IGraphicsContext> CreateContext(void* inNativeWindowPointer);
};

#endif // GRAPHICS_CONTEXT_H
