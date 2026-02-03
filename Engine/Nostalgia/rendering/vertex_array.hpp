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

    virtual void AddVertexBuffer(Shared<VertexBuffer> inVertexBuffer) = 0;
    virtual void SetIndexBuffer(Shared<IndexBuffer> inIndexBuffer) = 0;
    virtual Farg<std::vector<Shared<VertexBuffer>>> GetVertexBuffers() const = 0;
    virtual Shared<IndexBuffer> GetIndexBuffer() const = 0;

    static Shared<VertexArray> Create();

protected:
    std::vector<Shared<VertexBuffer>> mVertexBuffers{};
    Shared<IndexBuffer> mIndexBuffer{nullptr};
};

#endif // VERTEX_ARRAY_H
