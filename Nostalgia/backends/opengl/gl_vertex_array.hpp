#ifndef GL_VERTEX_ARRAY_H
#define GL_VERTEX_ARRAY_H

#include "rendering/vertex_array.hpp"

class OpenGLVertexArray : public VertexArray
{
public:
    OpenGLVertexArray();
    virtual ~OpenGLVertexArray();

    void Bind()   const final;
    void Unbind() const final;
    uint GetID()  const final;

    void AddVertexBuffer(Shared<VertexBuffer>) final;
    void SetIndexBuffer(Shared<IndexBuffer>) final;
    Shared<IndexBuffer> GetIndexBuffer() const final;

private:
    uint mObjectID{};
    uint mVertexAttributeIndex{0};
};

#endif // GL_VERTEX_ARRAY_H
