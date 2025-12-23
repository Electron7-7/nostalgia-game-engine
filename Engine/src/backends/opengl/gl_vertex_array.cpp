#include "gl_vertex_array.hpp"
#include "core/printing.hpp"
#include "application/application.hpp"

#include <glad/glad.h>

// Also stolen from Hazel [https://github.com/TheCherno/Hazel]

static GLenum sElementTypeToOpenGLType(IBuffer::Element::Type inType)
{
    switch(inType)
    {
    case IBuffer::Element::Type::Float:
    case IBuffer::Element::Type::Float2:
    case IBuffer::Element::Type::Float3:
    case IBuffer::Element::Type::Float4:
    case IBuffer::Element::Type::Mat3:
    case IBuffer::Element::Type::Mat4:
        return GL_FLOAT;
    case IBuffer::Element::Type::Int:
    case IBuffer::Element::Type::Int2:
    case IBuffer::Element::Type::Int3:
    case IBuffer::Element::Type::Int4:
        return GL_INT;
    case IBuffer::Element::Type::Bool:
        return GL_BOOL;
    default:
        print_error("Unknown IBuffer::Element::Type!");
        return 0;
    }
}

OpenGLVertexArray::OpenGLVertexArray()
{ glGenVertexArrays(1, &mObjectID); }

OpenGLVertexArray::~OpenGLVertexArray()
{ glDeleteVertexArrays(1, &mObjectID); }

void OpenGLVertexArray::Bind() const
{ glBindVertexArray(mObjectID); }

void OpenGLVertexArray::Unbind() const
{ glBindVertexArray(0); }

void OpenGLVertexArray::AddVertexBuffer(Shared<VertexBuffer> inVertexBuffer)
{
    if(inVertexBuffer->GetLayout().GetElements().size() == 0)
    {
        print_error("Vertex buffer layout has no elements!");
        return;
    }

    Bind();
    inVertexBuffer->Bind();

    const auto& layout{inVertexBuffer->GetLayout()};
    for(const auto& element : layout)
    {
        switch(element.type)
        {
        case IBuffer::Element::Type::Float:
        case IBuffer::Element::Type::Float2:
        case IBuffer::Element::Type::Float3:
        case IBuffer::Element::Type::Float4:
            glVertexAttribPointer(mVertexBufferIndex,
                element.GetCount(),
                sElementTypeToOpenGLType(element.type),
                element.normalized ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (void*)(element.offset));
            glEnableVertexAttribArray(mVertexBufferIndex);
            ++mVertexBufferIndex;
            break;
        case IBuffer::Element::Type::Int:
        case IBuffer::Element::Type::Int2:
        case IBuffer::Element::Type::Int3:
        case IBuffer::Element::Type::Int4:
        case IBuffer::Element::Type::Bool:
            glVertexAttribIPointer(mVertexBufferIndex,
                element.GetCount(),
                sElementTypeToOpenGLType(element.type),
                layout.GetStride(),
                (void*)(element.offset));
            glEnableVertexAttribArray(mVertexBufferIndex);
            ++mVertexBufferIndex;
            break;
        case IBuffer::Element::Type::Mat3:
        case IBuffer::Element::Type::Mat4:
        {
            uint count{element.GetCount()};
            for(uint i{0}; i < count; ++i)
            {
                glVertexAttribPointer(mVertexBufferIndex,
                    count,
                    sElementTypeToOpenGLType(element.type),
                    element.normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (void*)(element.offset + sizeof(float) * count * i));
                glVertexAttribDivisor(mVertexBufferIndex, 1);
                glEnableVertexAttribArray(mVertexBufferIndex);
                ++mVertexBufferIndex;
            }
            break;
        }
        case IBuffer::Element::Type::None:
        default:
            print_error("Unknown IBuffer::Element::Type!");
            return;
        }
    }

    mVertexBuffers.push_back(inVertexBuffer);
    inVertexBuffer->Unbind();
    Unbind();
}


void OpenGLVertexArray::SetIndexBuffer(Shared<IndexBuffer> inIndexBuffer)
{
    Bind();
    inIndexBuffer->Bind();
    mIndexBuffer = inIndexBuffer;
    Unbind();
}

Farg<std::vector<Shared<VertexBuffer>>> OpenGLVertexArray::GetVertexBuffers() const
{ return mVertexBuffers; }

Shared<IndexBuffer> OpenGLVertexArray::GetIndexBuffer() const
{ return mIndexBuffer; }
