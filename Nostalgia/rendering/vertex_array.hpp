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

    virtual void SetVertexBuffer(Shared<VertexBuffer> inVertexBuffer) = 0;
    virtual void SetIndexBuffer(Shared<IndexBuffer> inIndexBuffer) = 0;
    virtual Farg<Shared<VertexBuffer>> GetVertexBuffer() const = 0;
    virtual Farg<Shared<IndexBuffer>> GetIndexBuffer() const = 0;

    static Shared<VertexArray> Create();
};

#endif // VERTEX_ARRAY_H
