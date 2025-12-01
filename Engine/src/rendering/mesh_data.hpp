#ifndef MESH_DATA_H
#define MESH_DATA_H

#include "core/type_helpers.hpp"

#include <sys/types.h>
#include <string>
#include <vector>

// Inspired by Hazel [https:://github.com/TheCherno/Hazel]

class IBuffer
{
public:
    struct Element
    {
    public:
        enum class Type
        {
            None = 0,
            Float,  Float2, Float3, Float4,
            Int,    Int2,   Int3,   Int4,
            Mat3,   Mat4,
            Bool
        };

        std::string name{""};
        Type        type{Type::None};
        uint        size{0};
        size_t      offset{0};
        bool        normalized{false};

        constexpr Element() = default;
        constexpr Element(Type inType, FARG(std::string) inName, bool isNormalized = false):
            name{inName}, type{inType}, normalized{isNormalized} {}

        uint GetCount() const;

        static uint GetCount(Type inType);
        static uint GetSize(Type inType);
    };

    struct Layout
    {
    public:
        static constinit const Layout cIndexBufferLayout;

        typedef std::vector<Element> ElementList;

        Layout(std::initializer_list<Element> inElements = {});

        uint GetStride() const;
        FARG(ElementList) GetElements() const;
        ElementList::iterator begin();
        ElementList::iterator end();
        ElementList::const_iterator begin() const;
        ElementList::const_iterator end() const;

    private:
        ElementList mElements{};
        uint mStride{0};

        // Used by IndexBuffer
        constexpr Layout(bool): mStride{1} {}

        void CalculateOffsetsAndStride();
    };

    virtual void Bind()   = 0;
    virtual void Unbind() = 0;
    virtual void SetData(const void* inData, uint inSize) = 0;

    virtual FARG(Layout) GetLayout() const = 0;
    virtual void SetLayout(FARG(Layout) inLayout) = 0;

};

class VertexBuffer : public IBuffer
{
public:
    virtual ~VertexBuffer() = default;

    virtual void Bind()   override = 0;
    virtual void Unbind() override = 0;
    virtual void SetData(const void* inData, uint inSize) override = 0;

    virtual FARG(Layout) GetLayout() const override = 0;
    virtual void SetLayout(FARG(Layout) inLayout) override = 0;

    static Shared<VertexBuffer> Create(uint inSize);
    static Shared<VertexBuffer> Create(float* inVertices, uint inSize);
};

class IndexBuffer : public IBuffer
{
public:
    virtual ~IndexBuffer() = default;

    virtual void Bind()   override = 0;
    virtual void Unbind() override = 0;
    virtual uint GetCount() const  = 0;

    FARG(Layout) GetLayout() const final;
    void SetLayout(FARG(Layout)) final;
    void SetData(const void*, uint) final;

    static Shared<IndexBuffer> Create(uint* inIndices, uint inSize);

private:
    const Layout* mLayout{&Layout::cIndexBufferLayout};
};

struct MeshData
{
    Shared<VertexBuffer> vertex_buffer{nullptr};
    Shared<IndexBuffer>  index_buffer{nullptr};

    void CreateVertexBuffer(uint inSize);
    void CreateVertexBuffer(float* inVertices, uint inSize);
    void CreateIndexBuffer(uint* inIndices, uint inSize);
};

#endif // MESH_DATA_H
