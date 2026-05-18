#include "./primitive_mesh.hpp"
#include "./array_mesh.hpp"
#include "../thing_data.hpp"

// PrimitiveMesh
void PrimitiveMesh::InitVariables()
{ Super::InitVariables(); }

void PrimitiveMesh::SetVariables(Farg<TheatreFile::ThingData> data)
{
    Super::SetVariables(data);
    auto _mat{GetMaterial()};
    if(data.get_variable(_mat, "Material") == OK)
        { SetMaterial(_mat); }
}

Shared<TheatreFile::ThingData> PrimitiveMesh::GetVariables() const
{
    auto data{Super::GetVariables()};
    data->set_variable(GetMaterial(), "Material");
    return data;
}

Shared<Material> PrimitiveMesh::GetMaterial() const
{ return GetArrayMesh()->SurfaceGetMaterial(0); }

void PrimitiveMesh::SetMaterial(Shared<Material> inMaterial)
{ GetArrayMesh()->SurfaceSetMaterial(0, inMaterial); }

Shared<ArrayMesh> PrimitiveMesh::GetArrayMesh() const
{
    if(m_pArrayMesh->invalid())
        { UpdateMesh(); }
    return m_pArrayMesh;
}

void PrimitiveMesh::UpdateMesh() const
{ m_pArrayMesh = ArrayMesh::CreateFromMeshData(PRIMITIVE_TRIANGLES, CreateMeshData()); }

Shared<VertexArray> PrimitiveMesh::SurfaceGetVertexArray(int) const
{ return GetArrayMesh()->SurfaceGetVertexArray(0); }

Shared<Material> PrimitiveMesh::SurfaceGetMaterial(int) const
{ return GetMaterial(); }

// BoxMesh
void BoxMesh::InitVariables() { Super::InitVariables(); }

void BoxMesh::SetVariables(Farg<TheatreFile::ThingData> inData)
{
    Super::SetVariables(inData);

    if(inData.get_variable(mSize, "Size") == OK)
        { UpdateMesh(); }
}

Shared<TheatreFile::ThingData> BoxMesh::GetVariables() const
{
    auto _data{Super::GetVariables()};

    _data->set_variable(mSize, "Size");

    return _data;
}

glm::vec3 BoxMesh::GetSize() const
{ return mSize; }

void BoxMesh::SetSize(Farg<glm::vec3> inSize)
{
    mSize = inSize;
    UpdateMesh();
}

Mesh::MeshData_t BoxMesh::CreateMeshData() const
{
    MeshData_t _out{};
    auto _size{mSize * 0.5f};
    auto& _verts{_out.vertex_data[ARRAY_FORMAT_VERTEX]};
    auto& _normals{_out.vertex_data[ARRAY_FORMAT_NORMALS]};
    auto& _uvs{_out.vertex_data[ARRAY_FORMAT_UV]};

    _verts = {
        // Back face
        -_size[0], -_size[1], -_size[2], // BL
         _size[0],  _size[1], -_size[2], // TR
         _size[0], -_size[1], -_size[2], // BR
         _size[0],  _size[1], -_size[2], // TR
        -_size[0], -_size[1], -_size[2], // BL
        -_size[0],  _size[1], -_size[2], // TL
        // Front face
        -_size[0], -_size[1],  _size[2], // BL
         _size[0], -_size[1],  _size[2], // BR
         _size[0],  _size[1],  _size[2], // TR
         _size[0],  _size[1],  _size[2], // TR
        -_size[0],  _size[1],  _size[2], // TL
        -_size[0], -_size[1],  _size[2], // BL
        // Left face
        -_size[0],  _size[1],  _size[2], // TR
        -_size[0],  _size[1], -_size[2], // TL
        -_size[0], -_size[1], -_size[2], // BL
        -_size[0], -_size[1], -_size[2], // BL
        -_size[0], -_size[1],  _size[2], // BR
        -_size[0],  _size[1],  _size[2], // TR
        // Right face
         _size[0],  _size[1],  _size[2], // TL
         _size[0], -_size[1], -_size[2], // BR
         _size[0],  _size[1], -_size[2], // TR
         _size[0], -_size[1], -_size[2], // BR
         _size[0],  _size[1],  _size[2], // TL
         _size[0], -_size[1],  _size[2], // BL
        // Bottom face
        -_size[0], -_size[1], -_size[2], // TR
         _size[0], -_size[1], -_size[2], // TL
         _size[0], -_size[1],  _size[2], // BL
         _size[0], -_size[1],  _size[2], // BL
        -_size[0], -_size[1],  _size[2], // BR
        -_size[0], -_size[1], -_size[2], // TR
        // Top face
        -_size[0],  _size[1], -_size[2], // TL
         _size[0],  _size[1],  _size[2], // BR
         _size[0],  _size[1], -_size[2], // TR
         _size[0],  _size[1],  _size[2], // BR
        -_size[0],  _size[1], -_size[2], // TL
        -_size[0],  _size[1],  _size[2], // BL
    };

    _normals = {
        // Back face
        0.0f, 0.0f, -1.0f, // BL
        0.0f, 0.0f, -1.0f, // TR
        0.0f, 0.0f, -1.0f, // BR
        0.0f, 0.0f, -1.0f, // TR
        0.0f, 0.0f, -1.0f, // BL
        0.0f, 0.0f, -1.0f, // TL
        // Front face
        0.0f, 0.0f, 1.0f, // BL
        0.0f, 0.0f, 1.0f, // BR
        0.0f, 0.0f, 1.0f, // TR
        0.0f, 0.0f, 1.0f, // TR
        0.0f, 0.0f, 1.0f, // TL
        0.0f, 0.0f, 1.0f, // BL
        // Left face
        -1.0f, 0.0f, 0.0f, // TR
        -1.0f, 0.0f, 0.0f, // TL
        -1.0f, 0.0f, 0.0f, // BL
        -1.0f, 0.0f, 0.0f, // BL
        -1.0f, 0.0f, 0.0f, // BR
        -1.0f, 0.0f, 0.0f, // TR
        // Right face
        1.0f, 0.0f, 0.0f, // TL
        1.0f, 0.0f, 0.0f, // BR
        1.0f, 0.0f, 0.0f, // TR
        1.0f, 0.0f, 0.0f, // BR
        1.0f, 0.0f, 0.0f, // TL
        1.0f, 0.0f, 0.0f, // BL
        // Bottom face
        0.0f, -1.0f, 0.0f, // TR
        0.0f, -1.0f, 0.0f, // TL
        0.0f, -1.0f, 0.0f, // BL
        0.0f, -1.0f, 0.0f, // BL
        0.0f, -1.0f, 0.0f, // BR
        0.0f, -1.0f, 0.0f, // TR
        // Top face
        0.0f, 1.0f, 0.0f, // TL
        0.0f, 1.0f, 0.0f, // BR
        0.0f, 1.0f, 0.0f, // TR
        0.0f, 1.0f, 0.0f, // BR
        0.0f, 1.0f, 0.0f, // TL
        0.0f, 1.0f, 0.0f, // BL
    };

    _uvs = {
        // Back face
        1.0f, 0.0f, // BL
        0.0f, 1.0f, // TR
        0.0f, 0.0f, // BR
        0.0f, 1.0f, // TR
        1.0f, 0.0f, // BL
        1.0f, 1.0f, // TL
        // Front face
        0.0f, 0.0f, // BL
        1.0f, 0.0f, // BR
        1.0f, 1.0f, // TR
        1.0f, 1.0f, // TR
        0.0f, 1.0f, // TL
        0.0f, 0.0f, // BL
        // Left face
        1.0f, 1.0f, // TR
        0.0f, 1.0f, // TL
        0.0f, 0.0f, // BL
        0.0f, 0.0f, // BL
        1.0f, 0.0f, // BR
        1.0f, 1.0f, // TR
        // Right face
        0.0f, 1.0f, // TL
        1.0f, 0.0f, // BR
        1.0f, 1.0f, // TR
        1.0f, 0.0f, // BR
        0.0f, 1.0f, // TL
        0.0f, 0.0f, // BL
        // Bottom face
        1.0f, 1.0f, // TR
        0.0f, 1.0f, // TL
        0.0f, 0.0f, // BL
        0.0f, 0.0f, // BL
        1.0f, 0.0f, // BR
        1.0f, 1.0f, // TR
        // Top face
        0.0f, 1.0f, // TL
        1.0f, 0.0f, // BR
        1.0f, 1.0f, // TR
        1.0f, 0.0f, // BR
        0.0f, 1.0f, // TL
        0.0f, 0.0f, // BL
    };

    return _out;
}

// SphereMesh
void SphereMesh::InitVariables() { Super::InitVariables(); }

void SphereMesh::SetVariables(Farg<TheatreFile::ThingData> inData)
{
    Super::SetVariables(inData);

    if(inData.get_variable(mHeight, "Height") == OK
        or inData.get_variable(mRadius, "Radius") == OK
        or inData.get_variable(mInvertNormals, "InvertNormals") == OK)
        { UpdateMesh(); }
}

Shared<TheatreFile::ThingData> SphereMesh::GetVariables() const
{
    auto _data{Super::GetVariables()};

    _data->set_variable(mHeight, "Height");
    _data->set_variable(mRadius, "Radius");
    _data->set_variable(mInvertNormals, "InvertNormals");

    return _data;
}

float SphereMesh::GetHeight() const
{ return mHeight; }

void SphereMesh::SetHeight(float inHeight)
{
    mHeight = inHeight;
    UpdateMesh();
}

float SphereMesh::GetRadius() const
{ return mRadius; }

void SphereMesh::SetRadius(float inRadius)
{
    mRadius = inRadius;
    UpdateMesh();
}

bool SphereMesh::GetInvertNormals() const
{ return mInvertNormals; }

void SphereMesh::SetInvertNormals(bool inInvertNormals)
{
    mInvertNormals = inInvertNormals;
    UpdateMesh();
}

uint SphereMesh::GetHorizontalResolution() const
{ return mSectors; }

void SphereMesh::SetHorizontalResolution(uint inHorizontalResolution)
{
    mSectors = inHorizontalResolution;
    UpdateMesh();
}

uint SphereMesh::GetVerticalResolution() const
{ return mStacks; }

void SphereMesh::SetVerticalResolution(uint inVerticalResolution)
{
    mStacks = inVerticalResolution;
    UpdateMesh();
}

// https://www.songho.ca/opengl/gl_sphere.html#sphere
Mesh::MeshData_t SphereMesh::CreateMeshData() const
{
    MeshData_t _data{};
    auto& _verts{_data.vertex_data[ARRAY_FORMAT_VERTEX]};
    auto& _normals{_data.vertex_data[ARRAY_FORMAT_NORMALS]};
    auto& _uvs{_data.vertex_data[ARRAY_FORMAT_UV]};

    float x{}, y{}, z{}, xy{},
        nx{}, ny{}, nz{},
        _length_inv{(mInvertNormals) ? 1.0f / mRadius : 1.0f},
        s{}, t{};
    float _sector_step(2 * std::numbers::pi / mSectors);
    float _stack_step(std::numbers::pi / mStacks);
    float _sector_angle{}, _stack_angle{};

    for(int i{0}; i <= mStacks; ++i)
    {
        _stack_angle = std::numbers::pi / 2 - i * _stack_step;
        xy = mRadius * std::cosf(_stack_angle);
        z  = mHeight * 0.5f * std::sinf(_stack_angle);

        for(int j{0}; j <= mSectors; ++j)
        {
            _sector_angle = j * _sector_step;
            x = xy * std::cosf(_sector_angle);
            y = xy * std::sinf(_sector_angle);
            _verts.push_back(x);
            _verts.push_back(y);
            _verts.push_back(z);

            nx = x * _length_inv;
            ny = y * _length_inv;
            nz = z * _length_inv;
            _normals.push_back(nx);
            _normals.push_back(ny);
            _normals.push_back(nz);

            s = static_cast<float>(j) / mSectors;
            t = static_cast<float>(i) / mStacks;
            _uvs.push_back(s);
            _uvs.push_back(t);
        }
    }

    uint k1{0}, k2{0};
    for(int i{0}; i < mStacks; ++i)
    {
        k1 = i * (mSectors + 1);
        k2 = k1 + mSectors + 1;

        for(int j{0}; j < mSectors; ++j, ++k1, ++k2)
        {
            if(i)
            {
                _data.indices.push_back(k1);
                _data.indices.push_back(k2);
                _data.indices.push_back(k1 + 1);
            }

            if(i != (mStacks - 1))
            {
                _data.indices.push_back(k1 + 1);
                _data.indices.push_back(k2);
                _data.indices.push_back(k2 + 1);
            }
        }
    }
    return _data;
}

// CapsuleMesh
/*void CapsuleMesh::InitVariables() { Super::InitVariables(); }

void CapsuleMesh::SetVariables(Farg<TheatreFile::ThingData> inData)
{
    Super::SetVariables(inData);

    if(inData.get_variable(mHeight, "Height") == OK
        or inData.get_variable(mRadius, "Radius") == OK)
        { UpdateMesh(); }
}

Shared<TheatreFile::ThingData> CapsuleMesh::GetVariables() const
{
    auto _data{Super::GetVariables()};

    _data->set_variable(mHeight, "Height");
    _data->set_variable(mRadius, "Radius");

    return _data;
}

float CapsuleMesh::GetHeight() const
{ return mHeight; }

void CapsuleMesh::SetHeight(float inHeight)
{
    mHeight = inHeight;
    if(mRadius > mHeight * 0.5f)
        { mRadius = mHeight * 0.5f; }
    UpdateMesh();
}

float CapsuleMesh::GetRadius() const
{ return mRadius; }

void CapsuleMesh::SetRadius(float inRadius)
{
    mRadius = inRadius;
    if(mHeight < 2 * mRadius)
        { mHeight = 2 * mRadius; }
    UpdateMesh();
}

Mesh::MeshData_t CapsuleMesh::CreateMeshData() const
{}*/

// CylinderMesh
void CylinderMesh::InitVariables() { Super::InitVariables(); }

void CylinderMesh::SetVariables(Farg<TheatreFile::ThingData> inData)
{
    Super::SetVariables(inData);

    if(inData.get_variable(mHeight, "Height") == OK
        or inData.get_variable(mBottomRadius, "BottomRadius") == OK
        or inData.get_variable(mTopRadius, "TopRadius") == OK)
        { UpdateMesh(); }
}

Shared<TheatreFile::ThingData> CylinderMesh::GetVariables() const
{
    auto _data{Super::GetVariables()};

    _data->set_variable(mHeight, "Height");
    _data->set_variable(mBottomRadius, "BottomRadius");
    _data->set_variable(mTopRadius, "TopRadius");

    return _data;
}

float CylinderMesh::GetHeight() const
{ return mHeight; }

void CylinderMesh::SetHeight(float inHeight)
{
    mHeight = inHeight;
    UpdateMesh();
}

float CylinderMesh::GetBottomRadius() const
{ return mBottomRadius; }

void CylinderMesh::SetBottomRadius(float inBottomRadius)
{
    mBottomRadius = inBottomRadius;
    UpdateMesh();
}

float CylinderMesh::GetTopRadius() const
{ return mTopRadius; }

void CylinderMesh::SetTopRadius(float inTopRadius)
{
    mTopRadius = inTopRadius;
    UpdateMesh();
}

uint CylinderMesh::GetResolution() const
{ return mResolution; }

void CylinderMesh::SetResolution(uint inResolution)
{
    mResolution = inResolution;
    UpdateMesh();
}

// https://www.songho.ca/opengl/gl_cylinder.html
Mesh::MeshData_t CylinderMesh::CreateMeshData() const
{
    MeshData_t _data{};
    float _step(2 * std::numbers::pi / mResolution);
    float _angle{};
    std::vector<float> _unit_circle_verts{};

    // Unit Circle
    for(int i{0}; i <= mResolution; ++i)
    {
        _angle = i * _step;
        _unit_circle_verts.push_back(std::cos(_angle));
        _unit_circle_verts.push_back(std::sin(_angle));
        _unit_circle_verts.push_back(0.0f);
    }

    // Side Vertices
    for(int i{0}; i < 2; ++i)
    {
        float h = -mHeight / 2.0f + i * mHeight;
        float t = 1.0f - i;
        for(int j{0}, k{0}; j <= mResolution; ++j, k += 3)
        {
            float ux{_unit_circle_verts[k]};
            float uy{_unit_circle_verts[k+1]};
            float uz{_unit_circle_verts[k+2]};
            _data.vertex_data[ARRAY_FORMAT_VERTEX].push_back(ux * ((not i) ? mBottomRadius : mTopRadius));
            _data.vertex_data[ARRAY_FORMAT_VERTEX].push_back(h);
            _data.vertex_data[ARRAY_FORMAT_VERTEX].push_back(uy * ((not i) ? mBottomRadius : mTopRadius));
            _data.vertex_data[ARRAY_FORMAT_NORMALS].push_back(ux);
            _data.vertex_data[ARRAY_FORMAT_NORMALS].push_back(uz);
            _data.vertex_data[ARRAY_FORMAT_NORMALS].push_back(uy);
            _data.vertex_data[ARRAY_FORMAT_UV].push_back(t);
            _data.vertex_data[ARRAY_FORMAT_UV].push_back(static_cast<float>(j) / mResolution);
        }
    }

    // Starting index for base/top surface
    int _base_center_index{static_cast<int>(_data.vertex_data[ARRAY_FORMAT_VERTEX].size()) / 3};
    // For some reason, I have to add 2 to `_top_center_index` instead of 1, like in the post linked above
    int _top_center_index = _base_center_index + mResolution + 2;

    // Base & Top Vertices
    for(int i{0}; i < 2; ++i)
    {
        float h{-mHeight * 0.5f + i * mHeight};
        float nz(-1 + i * 2);
        _data.vertex_data[ARRAY_FORMAT_VERTEX].push_back(0.0f);
        _data.vertex_data[ARRAY_FORMAT_VERTEX].push_back(h);
        _data.vertex_data[ARRAY_FORMAT_VERTEX].push_back(0.0f);
        _data.vertex_data[ARRAY_FORMAT_NORMALS].push_back(0.0f);
        _data.vertex_data[ARRAY_FORMAT_NORMALS].push_back(nz);
        _data.vertex_data[ARRAY_FORMAT_NORMALS].push_back(0.0f);
        _data.vertex_data[ARRAY_FORMAT_UV].push_back(0.5f);
        _data.vertex_data[ARRAY_FORMAT_UV].push_back(0.5f);
        for(int j{0}, k{0}; j <= mResolution; ++j, k += 3)
        {
            float ux{_unit_circle_verts[k]};
            float uy{_unit_circle_verts[k+1]};
            _data.vertex_data[ARRAY_FORMAT_VERTEX].push_back(ux * ((not i) ? mBottomRadius : mTopRadius));
            _data.vertex_data[ARRAY_FORMAT_VERTEX].push_back(h);
            _data.vertex_data[ARRAY_FORMAT_VERTEX].push_back(uy * ((not i) ? mBottomRadius : mTopRadius));
            _data.vertex_data[ARRAY_FORMAT_NORMALS].push_back(0.0f);
            _data.vertex_data[ARRAY_FORMAT_NORMALS].push_back(nz);
            _data.vertex_data[ARRAY_FORMAT_NORMALS].push_back(0.0f);
            _data.vertex_data[ARRAY_FORMAT_UV].push_back(-ux * 0.5f + 0.5f);
            _data.vertex_data[ARRAY_FORMAT_UV].push_back(-uy * 0.5f + 0.5f);
        }
    }

    uint k1{0};
    uint k2(mResolution + 1);

    // Side Surface Indices
    for(int i{0}; i < mResolution; ++i, ++k1, ++k2)
    {
        _data.indices.push_back(k1);
        _data.indices.push_back(k1 + 1);
        _data.indices.push_back(k2);

        _data.indices.push_back(k2);
        _data.indices.push_back(k1 + 1);
        _data.indices.push_back(k2 + 1);
    }

    // Base Surface Indices
    for(int i{0}, k{_base_center_index + 1}; i < mResolution; ++i, ++k)
    {
        if(i < mResolution - 1)
        {
            _data.indices.push_back(_base_center_index);
            _data.indices.push_back(k + 1);
            _data.indices.push_back(k);
        }
        else
        {
            _data.indices.push_back(_base_center_index);
            _data.indices.push_back(_base_center_index + 1);
            _data.indices.push_back(k);
        }
    }

    // Top Surface Indices
    for(int i{0}, k{_top_center_index + 1}; i < mResolution; ++i, ++k)
    {
        if(i < mResolution - 1)
        {
            _data.indices.push_back(_top_center_index);
            _data.indices.push_back(k);
            _data.indices.push_back(k + 1);
        }
        else
        {
            _data.indices.push_back(_top_center_index);
            _data.indices.push_back(k);
            _data.indices.push_back(_top_center_index + 1);
        }
    }
    return _data;
}
