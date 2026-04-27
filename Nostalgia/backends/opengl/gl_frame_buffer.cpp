#include "./gl_frame_buffer.hpp"
#include "./gl_enum_conversions.hpp"
#include "rendering/texture_buffer.hpp"
#include "rendering/renderer_api.hpp"
#include "thirdparty/glad/glad.h"

#define ASSERT_API if(not RendererAPI::HasActiveInstance()) { return; }

static constexpr const char* _gl_fbo_msg_1{"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"};
static constexpr const char* _gl_fbo_msg_2{"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"};
static constexpr const char* _gl_fbo_msg_3{"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"};
static constexpr const char* _gl_fbo_msg_4{"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"};
static constexpr const char* _gl_fbo_msg_5{"GL_FRAMEBUFFER_UNSUPPORTED"};
static constexpr const char* _gl_fbo_msg_6{"UNKNOWN"};
static constexpr const char* _gl_fbo_msg_fmt{"FBO {} is incomplete: {}"};

static Error s_CheckStatus(bool inDoPrint, GLenum inStatus, uint inID)
{
    switch(inStatus)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        return OK;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        if(inDoPrint)
            { print_errorv(VERBOSE0, _gl_fbo_msg_fmt, inID, _gl_fbo_msg_1); }
        return FAILED;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        if(inDoPrint)
            { print_errorv(VERBOSE0, _gl_fbo_msg_fmt, inID, _gl_fbo_msg_2); }
        return FAILED;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        if(inDoPrint)
            { print_errorv(VERBOSE0, _gl_fbo_msg_fmt, inID, _gl_fbo_msg_3); }
        return FAILED;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        if(inDoPrint)
            { print_errorv(VERBOSE0, _gl_fbo_msg_fmt, inID, _gl_fbo_msg_4); }
        return FAILED;
    case GL_FRAMEBUFFER_UNSUPPORTED:
        if(inDoPrint)
            { print_errorv(VERBOSE0, _gl_fbo_msg_fmt, inID, _gl_fbo_msg_5); }
        return FAILED;
    default:
        if(inDoPrint)
            { print_errorv(VERBOSE0, _gl_fbo_msg_fmt, inID, _gl_fbo_msg_6); }
        return FAILED;
    }
}

OpenGLFrameBuffer::OpenGLFrameBuffer() noexcept
{
    ASSERT_API
    glCreateFramebuffers(1, &mBufferID);
}

OpenGLFrameBuffer::~OpenGLFrameBuffer() noexcept
{
    ASSERT_API
    glDeleteFramebuffers(1, &mBufferID);
}

void OpenGLFrameBuffer::Bind() const
{ glBindFramebuffer(GL_FRAMEBUFFER, mBufferID); }

void OpenGLFrameBuffer::Unbind() const
{ glBindFramebuffer(GL_FRAMEBUFFER, 0); }

uint OpenGLFrameBuffer::ID() const
{ return mBufferID; }

Error OpenGLFrameBuffer::Status(bool inPrintErrorMessage) const
{
    Error _read_status{s_CheckStatus(inPrintErrorMessage,
        glCheckNamedFramebufferStatus(mBufferID,  GL_READ_FRAMEBUFFER), mBufferID)};
    Error _draw_status{s_CheckStatus(inPrintErrorMessage,
        glCheckNamedFramebufferStatus(mBufferID, GL_DRAW_FRAMEBUFFER), mBufferID)};
    return (not _read_status) ? _read_status : _draw_status;
}

uint OpenGLFrameBuffer::AttachedTextureBufferID() const
{ return mAttachedTextureBufferID; }

bool OpenGLFrameBuffer::HasAttachedTextureBuffer() const
{ return mHasTextureBuffer; }

uint OpenGLFrameBuffer::AttachedRenderBufferID() const
{ return mAttachedRenderBufferID; }

bool OpenGLFrameBuffer::HasAttachedRenderBuffer() const
{ return mHasRenderBuffer; }

void OpenGLFrameBuffer::AttachTextureBuffer(Shared<TextureBuffer> inBuffer, BufferAttachment inAttachment)
{
    DetachTextureBuffer();
    glNamedFramebufferTexture(mBufferID, Convert::GL_BufferAttachment(inAttachment), inBuffer->ID(), 0);
    mAttachedTextureBufferID = inBuffer->ID();
    mHasTextureBuffer = true;
    if(not Status())
        { print_error("Failed to attach TextureBuffer to a FrameBuffer"); }
}

void OpenGLFrameBuffer::DetachTextureBuffer(BufferAttachment inAttachment)
{
#pragma message("TODO: check if this actually works, lmfao")
    glNamedFramebufferTexture(mBufferID, Convert::GL_BufferAttachment(inAttachment), 0, 0);
    mAttachedTextureBufferID = 0;
    mHasTextureBuffer = false;
}

void OpenGLFrameBuffer::AttachRenderBuffer(Shared<RenderBuffer> inBuffer, BufferAttachment inAttachment)
{
    DetachRenderBuffer();
    glNamedFramebufferRenderbuffer(mBufferID,
        Convert::GL_BufferAttachment(inAttachment),
        GL_RENDERBUFFER,
        inBuffer->ID());
    mAttachedRenderBufferID = inBuffer->ID();
    mHasRenderBuffer = true;
    if(not Status())
        { print_error("Failed to attach RenderBuffer to a FrameBuffer"); }
}

void OpenGLFrameBuffer::DetachRenderBuffer(BufferAttachment inAttachment)
{
    glNamedFramebufferRenderbuffer(mBufferID, Convert::GL_BufferAttachment(inAttachment), GL_RENDERBUFFER, 0);
    mAttachedRenderBufferID = 0;
    mHasRenderBuffer = false;
}
