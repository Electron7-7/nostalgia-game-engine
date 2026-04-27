#include "gl_mesh_buffers.hpp"
#include "thirdparty/glad/glad.h"
#include "managers/render_manager.hpp"

OpenGLVertexBuffer::OpenGLVertexBuffer(size_t inSize):
    mSize{static_cast<int>(inSize)}
{
    glCreateBuffers(1, &mBufferID);
    glNamedBufferStorage(mBufferID, inSize * sizeof(float), nullptr, GL_DYNAMIC_STORAGE_BIT);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(const float* inVertices, size_t inSize):
    mSize{static_cast<int>(inSize)}
{
    glCreateBuffers(1, &mBufferID);
    glNamedBufferStorage(mBufferID, inSize * sizeof(float), inVertices, GL_DYNAMIC_STORAGE_BIT);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    if(g_pRenderManager->GetAPI()->IsRunning())
        { glDeleteBuffers(1, &mBufferID); }
}

void OpenGLVertexBuffer::Bind() const
{ glBindBuffer(GL_ARRAY_BUFFER, mBufferID); }

void OpenGLVertexBuffer::Unbind() const
{ glBindBuffer(GL_ARRAY_BUFFER, 0); }

void OpenGLVertexBuffer::SetData(const void* inData, size_t inSize)
{
    glNamedBufferSubData(mBufferID, 0, inSize, inData);
    mSize = static_cast<int>(inSize);
}

void OpenGLVertexBuffer::QueryData(void*& outData, int* outSize) const
{
    glGetNamedBufferParameteriv(mBufferID, GL_BUFFER_SIZE, outSize);
    outData = malloc(*outSize);
    glGetNamedBufferSubData(mBufferID, 0, *outSize, outData);
}

int OpenGLVertexBuffer::GetSize() const
{ return mSize; }

uint OpenGLVertexBuffer::GetID() const
{ return mBufferID; }

void OpenGLVertexBuffer::GetLayout(Layout& outLayout) const
{ outLayout = mLayout; }

void OpenGLVertexBuffer::SetLayout(Farg<Layout> inLayout)
{ mLayout = inLayout; }

OpenGLIndexBuffer::OpenGLIndexBuffer(const uint* inIndices, size_t inCount):
    mCount{static_cast<uint>(inCount)}
{
    glCreateBuffers(1, &mBufferID);
    glNamedBufferStorage(mBufferID, inCount * sizeof(uint), inIndices, GL_DYNAMIC_STORAGE_BIT);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    if(g_pRenderManager->GetAPI()->IsRunning())
        { glDeleteBuffers(1, &mBufferID); }
}

void OpenGLIndexBuffer::Bind() const
{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferID); }

void OpenGLIndexBuffer::Unbind() const
{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

uint OpenGLIndexBuffer::GetID() const
{ return mBufferID; }

uint OpenGLIndexBuffer::GetCount() const
{ return mCount; }

void OpenGLIndexBuffer::QueryData(void*& outData, int* outSize) const
{
    glGetNamedBufferParameteriv(mBufferID, GL_BUFFER_SIZE, outSize);
    outData = malloc(*outSize);
    glGetNamedBufferSubData(mBufferID, 0, *outSize, outData);
}
