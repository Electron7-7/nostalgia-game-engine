#include "gl_mesh_data.hpp"

#include <glad/glad.h>

OpenGLVertexBuffer::OpenGLVertexBuffer(uint inSize)
{
    glCreateBuffers(1, &mBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
    glBufferData(GL_ARRAY_BUFFER, inSize, nullptr, GL_DYNAMIC_DRAW);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(float* inVertices, uint inSize)
{
    glCreateBuffers(1, &mBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
    glBufferData(GL_ARRAY_BUFFER, inSize, inVertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{ glDeleteBuffers(1, &mBufferID); }

void OpenGLVertexBuffer::Bind()
{ glBindBuffer(GL_ARRAY_BUFFER, mBufferID); }

void OpenGLVertexBuffer::Unbind()
{ glBindBuffer(GL_ARRAY_BUFFER, 0); }

void OpenGLVertexBuffer::SetData(const void* inData, uint inSize)
{
    Bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, inSize, inData);
}

OpenGLIndexBuffer::OpenGLIndexBuffer(uint* inIndices, uint inCount):
    mCount{inCount}
{
    // Comment from Hazel:
    //      GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
    //      Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
    glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
    glBufferData(GL_ARRAY_BUFFER, inCount * sizeof(uint32_t), inIndices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{ glDeleteBuffers(1, &mBufferID); }

void OpenGLIndexBuffer::Bind()
{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferID); }

void OpenGLIndexBuffer::Unbind()
{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
