#include "gl_vertex_array.hpp"
#include "thirdparty/glad/glad.h"
#include "rendering/renderer_api.hpp"

#define ASSERT_API if(not RendererAPI::HasActiveInstance()) { return; }

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
{ glCreateVertexArrays(1, &mObjectID); }

OpenGLVertexArray::~OpenGLVertexArray()
{
    ASSERT_API
    glDeleteVertexArrays(1, &mObjectID);
}

void OpenGLVertexArray::Bind() const
{ glBindVertexArray(mObjectID); }

void OpenGLVertexArray::Unbind() const
{ glBindVertexArray(0); }

uint OpenGLVertexArray::GetID() const
{ return mObjectID; }

void OpenGLVertexArray::AddVertexBuffer(Shared<VertexBuffer> inVertexBuffer)
{
    IBuffer::Layout layout{};
    inVertexBuffer->GetLayout(layout);

    if(layout.GetElements().empty())
    {
        print_error("Vertex buffer layout has no elements!");
        return;
    }

    glVertexArrayVertexBuffer(mObjectID, mVertexBuffers.size(), inVertexBuffer->GetID(), 0, layout.GetStride());

    for(FAUTO element : layout)
    {
        glEnableVertexArrayAttrib(mObjectID, mVertexAttributeIndex);
        switch(element.type)
        {
        case IBuffer::Element::Type::Float:
        case IBuffer::Element::Type::Float2:
        case IBuffer::Element::Type::Float3:
        case IBuffer::Element::Type::Float4:
            glVertexArrayAttribFormat(mObjectID,
                mVertexAttributeIndex,
                element.GetCount(),
                sElementTypeToOpenGLType(element.type),
                element.normalized ? GL_TRUE : GL_FALSE,
                element.offset);
            break;
        case IBuffer::Element::Type::Int:
        case IBuffer::Element::Type::Int2:
        case IBuffer::Element::Type::Int3:
        case IBuffer::Element::Type::Int4:
        case IBuffer::Element::Type::Bool:
            glVertexArrayAttribIFormat(mObjectID,
                mVertexAttributeIndex,
                element.GetCount(),
                sElementTypeToOpenGLType(element.type),
                element.offset);
            break;
        case IBuffer::Element::Type::Mat3:
        case IBuffer::Element::Type::Mat4:
        {
            uint count{element.GetCount()};
            for(uint i{0}; i < count; ++i)
            {
                glVertexArrayAttribFormat(mObjectID,
                    mVertexAttributeIndex,
                    count,
                    sElementTypeToOpenGLType(element.type),
                    element.normalized ? GL_TRUE : GL_FALSE,
                    element.offset + sizeof(float) * count * i);
                glVertexArrayBindingDivisor(mObjectID, mVertexAttributeIndex, 1);
            }
            break;
        }
        case IBuffer::Element::Type::None:
        default:
            print_error("Unknown IBuffer::Element::Type!");
            return;
        }
        glVertexArrayAttribBinding(mObjectID, mVertexAttributeIndex++, mVertexBuffers.size());
    }
    mVertexBuffers.push_back(inVertexBuffer);
}

void OpenGLVertexArray::SetIndexBuffer(Shared<IndexBuffer> inIndexBuffer)
{
    glVertexArrayElementBuffer(mObjectID, inIndexBuffer->GetID());
    mIndexBuffer = inIndexBuffer;
}

Shared<IndexBuffer> OpenGLVertexArray::GetIndexBuffer() const
{ return mIndexBuffer; }
