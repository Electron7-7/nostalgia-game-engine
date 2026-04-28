#ifndef GL_MESH_DATA_H
#define GL_MESH_DATA_H

#include "rendering/buffers.hpp"

class OpenGLVertexBuffer final : public VertexBuffer
{
public:
    OpenGLVertexBuffer(size_t);
    OpenGLVertexBuffer(const float*, size_t);
    virtual ~OpenGLVertexBuffer();

    void Bind() const final;
    void Unbind() const final;
    void SetData(const void*, size_t) final;
    int GetSize() const final;
    uint GetID() const final;
    void GetLayout(Layout&) const final;
    void SetLayout(Farg<Layout>) final;

    void QueryData(void*& outData, int* outSize) const final;

private:
    int    mSize{};
    uint   mBufferID{};
    Layout mLayout{};
};

class OpenGLIndexBuffer final : public IndexBuffer
{
public:
    OpenGLIndexBuffer(const uint*, size_t);
    virtual ~OpenGLIndexBuffer();

    void Bind() const final;
    void Unbind() const final;
    uint GetCount() const final;
    uint GetID() const final;

    void QueryData(void*& outData, int* outSize) const final;

private:
    uint mBufferID{};
    uint mCount{0};
};

#endif // GL_MESH_DATA_H
