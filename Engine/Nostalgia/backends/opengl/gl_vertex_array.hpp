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

    void AddVertexBuffer(Shared<VertexBuffer>) final;
    void SetIndexBuffer(Shared<IndexBuffer>) final;
    Farg<std::vector<Shared<VertexBuffer>>> GetVertexBuffers() const final;
    Shared<IndexBuffer> GetIndexBuffer() const final;

private:
    uint mObjectID{};
    uint mVertexBufferIndex{0};
};

#endif // GL_VERTEX_ARRAY_H
