#include "./gl_render_buffer.hpp"
#include "./gl_enum_conversions.hpp"
#include "rendering/renderer_api.hpp"
#include "thirdparty/glad/glad.h"

#define ASSERT_API if(not RendererAPI::HasActiveInstance()) { return; }

OpenGLRenderBuffer::OpenGLRenderBuffer()
{
    ASSERT_API
    glCreateRenderbuffers(1, &mBufferID);
}

OpenGLRenderBuffer::~OpenGLRenderBuffer()
{
    ASSERT_API
    glDeleteRenderbuffers(1, &mBufferID);
}

uint OpenGLRenderBuffer::ID()
{ return mBufferID; }

void OpenGLRenderBuffer::SetStorage(Farg<Size2D> inSize, DataFormat inComponent)
{
    ASSERT_API
    glNamedRenderbufferStorage(mBufferID, Convert::GL_DataFormat(inComponent, true), inSize.w(), inSize.h());
}
