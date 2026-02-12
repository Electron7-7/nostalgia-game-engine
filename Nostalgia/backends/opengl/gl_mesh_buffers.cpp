#include "gl_mesh_buffers.hpp"
#include "thirdparty/glad/glad.h"

OpenGLVertexBuffer::OpenGLVertexBuffer(size_t inSize)
{
    glGenBuffers(1, &mBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
    glBufferData(GL_ARRAY_BUFFER, inSize * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(float* inVertices, size_t inSize)
{
    glGenBuffers(1, &mBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
    glBufferData(GL_ARRAY_BUFFER, inSize * sizeof(float), inVertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{ glDeleteBuffers(1, &mBufferID); }

void OpenGLVertexBuffer::Bind() const
{ glBindBuffer(GL_ARRAY_BUFFER, mBufferID); }

void OpenGLVertexBuffer::Unbind() const
{ glBindBuffer(GL_ARRAY_BUFFER, 0); }

void OpenGLVertexBuffer::SetData(const void* inData, size_t inSize)
{
    Bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, inSize, inData);
}

uint OpenGLVertexBuffer::GetID() const
{ return mBufferID; }

Farg<IBuffer::Layout> OpenGLVertexBuffer::GetLayout() const
{ return mLayout; }

void OpenGLVertexBuffer::SetLayout(Farg<Layout> inLayout)
{ mLayout = inLayout; }

OpenGLIndexBuffer::OpenGLIndexBuffer(uint* inIndices, size_t inCount):
    mCount{static_cast<uint>(inCount)}
{
    glGenBuffers(1, &mBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
    glBufferData(GL_ARRAY_BUFFER, inCount * sizeof(uint), inIndices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{ glDeleteBuffers(1, &mBufferID); }

void OpenGLIndexBuffer::Bind() const
{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferID); }

void OpenGLIndexBuffer::Unbind() const
{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

uint OpenGLIndexBuffer::GetID() const
{ return mBufferID; }

uint OpenGLIndexBuffer::GetCount() const
{ return mCount; }
