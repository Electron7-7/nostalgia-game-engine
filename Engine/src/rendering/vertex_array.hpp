#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

// Stolen from Hazel [https://github.com/TheCherno/Hazel]

#include "mesh_data.hpp"
#include "core/type_helpers.hpp"

class VertexArray
{
public:
    virtual ~VertexArray() = default;

    virtual void Bind()   const = 0;
    virtual void Unbind() const = 0;

    virtual void AddVertexBuffer(FARG(Shared<VertexBuffer>) inVertexBuffer) = 0;
    virtual void SetIndexBuffer(FARG(Shared<IndexBuffer>) inIndexBuffer) = 0;

    virtual FARG(std::vector<Shared<VertexBuffer>>) GetVertexBuffers() const = 0;
    virtual FARG(Shared<IndexBuffer>) GetIndexBuffer() const = 0;

    static Shared<VertexArray> Create();

protected:
    std::vector<Shared<VertexBuffer>> mVertexBuffers{};
    Shared<IndexBuffer> mIndexBuffer{nullptr};
};

#endif // VERTEX_ARRAY_H
