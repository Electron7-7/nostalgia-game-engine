#include "mesh_buffers.hpp"
#include "rendering/renderer_api.hpp"
// Implementations
#include "backends/opengl/gl_mesh_buffers.hpp"

const IBuffer::Layout IBuffer::Layout::cIndexBufferLayout{{Element::Type::Int, "FaceIndex"}};
const IBuffer::Layout IBuffer::Layout::cDefaultMeshLayout{
    {Element::Type::Float3, "Position"},
    {Element::Type::Float3, "Color"},
    {Element::Type::Float3, "Normal"},
    {Element::Type::Float2, "UV"},
};

uint IBuffer::Element::GetComponentCount(Type inType)
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

uint IBuffer::Element::GetComponentSize(Type inType)
{
    switch(inType)
    {
    case Type::Bool:
        return 1;
    case Type::Mat3:
        return sizeof(float) * 3 * 3;
    case Type::Mat4:
        return sizeof(float) * 4 * 4;
    default:
        return sizeof(float) * GetComponentCount(inType);
    }
}

uint IBuffer::Element::GetCount() const
{ return GetComponentCount(type); }

void IBuffer::Element::DebugLog() const
{ print_debug("Element {}: size: {}, offset: {}", name, size, offset); }

IBuffer::Layout::Layout():
    Layout{cDefaultMeshLayout} {}

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

void IBuffer::Layout::DebugLog() const
{
    print_debug("Layout: stride: {}", mStride);
    for(auto element : mElements)
        { element.DebugLog(); }
}

void IBuffer::Layout::CalculateOffsetsAndStride()
{
    size_t offset{0};
    mStride = 0;
    for(auto& element : mElements)
    {
        element.offset = offset;
        offset  += element.size;
        mStride += element.size;
    }
}

Shared<VertexBuffer> VertexBuffer::Create(size_t inSize)
{
    std::string error_api_name{"GraphicsAPI::None"};
    switch(RendererAPI::GetAPI())
    {
    default:
        error_api_name = "Invalid";
        [[fallthrough]];
    case GraphicsAPI::None:
        print_warning("RendererAPI::GetAPI() returned '{}' (defaulting to OpenGL)", error_api_name);
        [[fallthrough]];
    case GraphicsAPI::OpenGL:
        return MakeShared<OpenGLVertexBuffer>(inSize);
    }
}

Shared<VertexBuffer> VertexBuffer::Create(float* inVertices, size_t inSize)
{
    std::string error_api_name{"GraphicsAPI::None"};
    switch(RendererAPI::GetAPI())
    {
    default:
        error_api_name = "Invalid";
        [[fallthrough]];
    case GraphicsAPI::None:
        print_warning("RendererAPI::GetAPI() returned '{}' (defaulting to OpenGL)", error_api_name);
        [[fallthrough]];
    case GraphicsAPI::OpenGL:
        return MakeShared<OpenGLVertexBuffer>(inVertices, inSize);
    }
}

Shared<IndexBuffer> IndexBuffer::Create(uint* inIndices, size_t inSize)
{
    std::string error_api_name{"GraphicsAPI::None"};
    switch(RendererAPI::GetAPI())
    {
    default:
        error_api_name = "Invalid";
        [[fallthrough]];
    case GraphicsAPI::None:
        print_warning("RendererAPI::GetAPI() returned '{}' (defaulting to OpenGL)", error_api_name);
        [[fallthrough]];
    case GraphicsAPI::OpenGL:
        return MakeShared<OpenGLIndexBuffer>(inIndices, inSize);
    }
}
