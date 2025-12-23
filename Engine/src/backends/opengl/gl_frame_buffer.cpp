#include "gl_frame_buffer.hpp"
#include "core/printing.hpp"
#include "rendering/texture_buffer.hpp"
#include <glad/glad.h>

OpenGLFrameBuffer::~OpenGLFrameBuffer() noexcept
{ glDeleteFramebuffers(1, &mFrameBufferID); }

uint OpenGLFrameBuffer::Generate()
{
    glGenFramebuffers(1, &mFrameBufferID);
    return mFrameBufferID;
}

Error OpenGLFrameBuffer::SetOutputTexture(Shared<TextureBuffer> inTexture, ID inID)
{
    Error status{OK};
    mTextureBufferID = inID;
    mTextureID = inTexture->GetID();
    Bind();
    switch(inTexture->GetFormat().type)
    {
    case TEXTURE_TYPE_2D:
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureID, 0);
        break;
    default:
        print_warning("Texture types other than GL_TEXTURE_2D not implemented for OpenGLFrameBuffer");
        status = UNIMPLEMENTED;
        mTextureID = 0;
        mTextureBufferID = ID::Invalid;
        break;
    }
    Unbind();
    return status;
}

ID OpenGLFrameBuffer::GetOutputTextureID() const
{ return mTextureID; }

Farg<Scale2D> OpenGLFrameBuffer::GetScale() const
{ return mTextureScale; }

void OpenGLFrameBuffer::Bind() const
{ glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferID); }

void OpenGLFrameBuffer::Unbind() const
{ glBindFramebuffer(GL_FRAMEBUFFER, 0); }

uint OpenGLFrameBuffer::GetID() const
{ return mFrameBufferID; }
