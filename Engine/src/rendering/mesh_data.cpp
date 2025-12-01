#include "mesh_data.hpp"
#include "core/printing.hpp"
#include "rendering/renderer_api.hpp"

// Implementations
#include "backends/opengl/gl_mesh_data.hpp"

constinit const IBuffer::Layout IBuffer::Layout::cIndexBufferLayout{true};

uint IBuffer::Element::GetCount(Type inType)
{
    switch(inType)
    {
        case Type::Bool:
        case Type::Int:
        case Type::Float:
            return 1;
        case Type::Int2:
        case Type::Float2:
            return 2;
        case Type::Mat3:
        case Type::Int3:
        case Type::Float3:
            return 3;
        case Type::Mat4:
        case Type::Int4:
        case Type::Float4:
            return 4;
        default:
            print_warning("Unknown Element type!");
            return 0;
    }
}

uint IBuffer::Element::GetSize(Type inType)
{
    switch(inType)
    {
    case Type::Bool:
        return 1;
    case Type::Mat3:
        return 4 * 3 * 3;
    case Type::Mat4:
        return 4 * 4 * 4;
    default:
        return 4 * GetCount(inType);
    }
}

uint IBuffer::Element::GetCount() const
{ return GetCount(type); }

IBuffer::Layout::Layout(std::initializer_list<Element> inElements):
    mElements{inElements}
{ CalculateOffsetsAndStride(); }

uint IBuffer::Layout::GetStride() const
{ return mStride; }

const IBuffer::Layout::ElementList& IBuffer::Layout::GetElements() const
{ return mElements; }

IBuffer::Layout::ElementList::iterator IBuffer::Layout::begin()
{ return mElements.begin(); }

IBuffer::Layout::ElementList::iterator IBuffer::Layout::end()
{ return mElements.end(); }

IBuffer::Layout::ElementList::const_iterator IBuffer::Layout::begin() const
{ return mElements.cbegin(); }

IBuffer::Layout::ElementList::const_iterator IBuffer::Layout::end() const
{ return mElements.cend(); }

void IBuffer::Layout::CalculateOffsetsAndStride()
{
    size_t offset{0};
    mStride = 0;
    for(auto& element : mElements)
    {
        element.offset = offset;
        offset += element.size;
        mStride += element.size;
    }
}

Shared<VertexBuffer> VertexBuffer::Create(uint inSize)
{
    std::string error_api_name{"GraphicsAPI::None"};
    switch(IRendererAPI::GetAPI())
    {
    default:
        error_api_name = "Invalid";
        [[fallthrough]];
    case GraphicsAPI::None:
        print_warning("IRendererAPI::GetAPI() returned '{}' (defaulting to OpenGL)", error_api_name);
        [[fallthrough]];
    case GraphicsAPI::OpenGL:
        return MakeShared<OpenGLVertexBuffer>(inSize);
    }
}

Shared<VertexBuffer> VertexBuffer::Create(float* inVertices, uint inSize)
{
    std::string error_api_name{"GraphicsAPI::None"};
    switch(IRendererAPI::GetAPI())
    {
    default:
        error_api_name = "Invalid";
        [[fallthrough]];
    case GraphicsAPI::None:
        print_warning("IRendererAPI::GetAPI() returned '{}' (defaulting to OpenGL)", error_api_name);
        [[fallthrough]];
    case GraphicsAPI::OpenGL:
        return MakeShared<OpenGLVertexBuffer>(inVertices, inSize);
    }
}

FARG(IBuffer::Layout) IndexBuffer::GetLayout() const
{ return *mLayout; }

void IndexBuffer::SetLayout(FARG(Layout))
{}

void IndexBuffer::SetData(const void*, uint)
{}

void MeshData::CreateVertexBuffer(uint inSize)
{ vertex_buffer = VertexBuffer::Create(inSize); }

void MeshData::CreateVertexBuffer(float* inVertices, uint inSize)
{ vertex_buffer = VertexBuffer::Create(inVertices, inSize); }

void MeshData::CreateIndexBuffer(uint* inIndices, uint inSize)
{ index_buffer = IndexBuffer::Create(inIndices, inSize); }
