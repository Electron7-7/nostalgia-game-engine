#include "gl_frame_buffer.hpp"
#include "rendering/texture_buffer.hpp"
#include "thirdparty/glad/glad.h"

static Error s_CheckFramebufferStatus(uint inID)
{
    switch(glCheckFramebufferStatus(GL_FRAMEBUFFER))
    {
    case GL_FRAMEBUFFER_COMPLETE:
        return OK;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        print_warning("FrameBuffer#{} incomplete (GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT): Attachment is NOT complete.", inID);
        return FAILED;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        print_warning("FrameBuffer#{} incomplete (GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT): No image is attached to FBO.", inID);
        return FAILED;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        print_warning("FrameBuffer#{} incomplete (GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER): Draw buffer.", inID);
        return FAILED;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        print_warning("FrameBuffer#{} incomplete (GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER): Read buffer.", inID);
        return FAILED;
    case GL_FRAMEBUFFER_UNSUPPORTED:
        print_warning("FrameBuffer#{} incomplete (GL_FRAMEBUFFER_UNSUPPORTED): Unsupported by FBO implementation.", inID);
        return FAILED;
    default:
        print_warning("FrameBuffer#{} incomplete: Unknown error.", inID);
        return FAILED;
    }
}

OpenGLFrameBuffer::OpenGLFrameBuffer() noexcept:
    mBufferID{0}, mStatus{OK} {}

OpenGLFrameBuffer::OpenGLFrameBuffer(Farg<Size2D> inSize) noexcept:
    mStatus{OK}
{
    glGenFramebuffers(1, &mBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);

    mTextureBuffer = TextureBuffer::Create();
    mTextureBuffer->Load(nullptr, {inSize.w(), inSize.h(), DATA_FORMAT_SRGB_ALPHA});
    mTextureBuffer->GenerateMipMaps();

    glGenRenderbuffers(1, &mRenderBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, mRenderBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, inSize.w(), inSize.h());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRenderBufferID);
    glNamedFramebufferTexture(mBufferID, GL_COLOR_ATTACHMENT0, mTextureBuffer->ID(), 0);

    mStatus = s_CheckFramebufferStatus(mBufferID);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OpenGLFrameBuffer::~OpenGLFrameBuffer() noexcept
{ if(mStatus == OK and mBufferID) { glDeleteFramebuffers(1, &mBufferID); } }

uint OpenGLFrameBuffer::TextureID() const
{ return mTextureBuffer->ID(); }

Shared<TextureBuffer> OpenGLFrameBuffer::Texture() const
{ return mTextureBuffer; }

uint OpenGLFrameBuffer::RenderBufferID() const
{ return mRenderBufferID; }

void OpenGLFrameBuffer::Bind() const
{ glBindFramebuffer(GL_FRAMEBUFFER, mBufferID); }

void OpenGLFrameBuffer::Unbind() const
{ glBindFramebuffer(GL_FRAMEBUFFER, 0); }

uint OpenGLFrameBuffer::ID() const
{ return mBufferID; }

Error OpenGLFrameBuffer::Status() const
{ return mStatus; }
