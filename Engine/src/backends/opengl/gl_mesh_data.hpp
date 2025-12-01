#ifndef GL_MESH_DATA_H
#define GL_MESH_DATA_H

#include "rendering/mesh_data.hpp"

class OpenGLVertexBuffer : public VertexBuffer
{
public:
    OpenGLVertexBuffer(uint inSize);
    OpenGLVertexBuffer(float* inVertices, uint inSize);
    virtual ~OpenGLVertexBuffer();

    void Bind() final;
    void Unbind() final;
    void SetData(const void* inData, uint inSize) final;
    FARG(Layout) GetLayout() const final;
    void SetLayout(FARG(Layout) inLayout) final;

private:
    uint   mBufferID{};
    Layout mLayout{};
};

class OpenGLIndexBuffer : public IndexBuffer
{
public:
    OpenGLIndexBuffer(uint* inIndices, uint inSize);
    virtual ~OpenGLIndexBuffer();

    void Bind() final;
    void Unbind() final;
    uint GetCount() const final;

private:
    uint mBufferID{};
    uint mCount{0};
};

#endif // GL_MESH_DATA_H
