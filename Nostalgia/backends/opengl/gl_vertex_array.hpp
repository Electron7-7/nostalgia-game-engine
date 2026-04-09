#ifndef GL_VERTEX_ARRAY_H
#define GL_VERTEX_ARRAY_H

#include <Nostalgia/rendering/vertex_array.hpp>

class OpenGLVertexArray : public VertexArray
{
public:
    OpenGLVertexArray();
    virtual ~OpenGLVertexArray();

    void Bind()   const final;
    void Unbind() const final;
    uint GetID()  const final;

    void SetVertexBuffer(Shared<VertexBuffer>) final;
    void SetIndexBuffer(Shared<IndexBuffer>) final;
    Farg<Shared<VertexBuffer>> GetVertexBuffer() const final;
    Farg<Shared<IndexBuffer>> GetIndexBuffer() const final;

private:
    uint mObjectID{};
    Shared<VertexBuffer> mVertexBuffer{nullptr};
    Shared<IndexBuffer> mIndexBuffer{nullptr};
};

#endif // GL_VERTEX_ARRAY_H
