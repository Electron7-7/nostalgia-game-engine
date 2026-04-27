#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

// Stolen from Hazel [https://github.com/TheCherno/Hazel]

#include <Nostalgia/rendering/mesh_buffers.hpp>

class VertexArray
{
public:
    virtual ~VertexArray() = default;

    virtual void Bind()   const = 0;
    virtual void Unbind() const = 0;
    virtual uint GetID()  const = 0;

    virtual void AddVertexBuffer(Shared<VertexBuffer> inVertexBuffer) = 0;
    virtual void SetIndexBuffer(Shared<IndexBuffer> inIndexBuffer) = 0;
    virtual Shared<IndexBuffer> GetIndexBuffer() const = 0;

    static Shared<VertexArray> CreateDummy();
    static Shared<VertexArray> Create();

protected:
    std::vector<Shared<VertexBuffer>> mVertexBuffers{};
    Shared<IndexBuffer> mIndexBuffer{nullptr};
};

class DummyVertexArray final : public VertexArray
{
public:
    DummyVertexArray() = default;
    virtual ~DummyVertexArray() = default;

    void Bind()   const final {}
    void Unbind() const final {}
    uint GetID()  const final { return 0; }

    void AddVertexBuffer(Shared<VertexBuffer>) final {}
    void SetIndexBuffer(Shared<IndexBuffer>) final {}
    Shared<IndexBuffer> GetIndexBuffer() const final { return IndexBuffer::CreateDummy(); }
};

#endif // VERTEX_ARRAY_H
