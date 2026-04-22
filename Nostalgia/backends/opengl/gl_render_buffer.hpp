#ifndef GL_RENDER_BUFFER_H
#define GL_RENDER_BUFFER_H

#include "rendering/render_buffer.hpp"

class OpenGLRenderBuffer : public RenderBuffer
{
public:
    OpenGLRenderBuffer();
    OpenGLRenderBuffer(Farg<Size2D>, DataFormat = DATA_FORMAT_DEPTH);
    ~OpenGLRenderBuffer();

    uint ID() final;
    void SetStorage(Farg<Size2D> inSize, DataFormat = DATA_FORMAT_DEPTH) final;

private:
    uint mBufferID{0};
};

#endif // GL_RENDER_BUFFER_H
