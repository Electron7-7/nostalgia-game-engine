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
    void AddVertexBuffer(FARG(Shared<VertexBuffer>)) final;
    void SetIndexBuffer(FARG(Shared<IndexBuffer>)) final;
    FARG(std::vector<Shared<VertexBuffer>>) GetVertexBuffers() const final;
    FARG(Shared<IndexBuffer>) GetIndexBuffer() const final;

private:
    uint mObjectID{};
    uint mVertexBufferIndex{0};
};

#endif // GL_VERTEX_ARRAY_H
