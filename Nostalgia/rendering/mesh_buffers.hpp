#ifndef MESH_BUFFERS_H
#define MESH_BUFFERS_H

// Taken from Hazel [https:://github.com/TheCherno/Hazel]

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

        Element() = default;
        Element(Type inType, Farg<std::string> inName, bool isNormalized = false):
            name{inName}, type{inType}, size{GetComponentSize(inType)}, offset{0}, normalized{isNormalized} {}

        uint GetCount() const;
        void DebugLog() const;

        static uint GetComponentCount(Type inType);
        static uint GetComponentSize(Type inType);
    };

    struct Layout
    {
    public:
        static const Layout cIndexBufferLayout;
        static const Layout cDefaultMeshLayout;

        typedef std::vector<Element> ElementList;

        Layout();
        Layout(std::initializer_list<Element> inElements);

        uint GetStride() const;
        Farg<ElementList> GetElements() const;
        ElementList::iterator begin();
        ElementList::iterator end();
        ElementList::const_iterator begin() const;
        ElementList::const_iterator end() const;
        void DebugLog() const;

    private:
        ElementList mElements{};
        uint mStride{0};

        void CalculateOffsetsAndStride();
    };

    virtual void Bind()   const = 0;
    virtual void Unbind() const = 0;
    virtual void SetData(const void* inData, size_t inSize) = 0;
    virtual uint GetID() const = 0;
    virtual Farg<Layout> GetLayout() const = 0;
    virtual void SetLayout(Farg<Layout> inLayout) = 0;

    virtual void QuerySize(int*) const {}
    virtual void QueryData(void*& outData, int* outSize) const {}
};

class VertexBuffer : public IBuffer
{
public:
    virtual ~VertexBuffer() = default;

    virtual int GetSize() const = 0;

    static Shared<VertexBuffer> Create(size_t inSize);
    static Shared<VertexBuffer> Create(float* inVertices, size_t inSize);
};

class IndexBuffer : public IBuffer
{
public:
    virtual ~IndexBuffer() = default;

    virtual uint GetCount() const = 0;

    static Shared<IndexBuffer> Create(uint* inIndices, size_t inSize);

private:
    Farg<Layout> GetLayout() const final { return Layout::cIndexBufferLayout; }
    void SetLayout(Farg<Layout>) final {}
    void SetData(const void*, size_t) final {}
};

#endif // MESH_BUFFERS_H
