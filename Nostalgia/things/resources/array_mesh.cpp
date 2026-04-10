#include "./array_mesh.hpp"
#include "things/thing_factory.hpp"
#include "things/thing_data.hpp"
#include "rendering/mesh_buffers.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include "thirdparty/TinyOBJLoader/tiny_obj_loader.h"

#define ASSERT_SURFACE_INDEX(INDEX, ...) if(INDEX >= mSurfaces.size()) { return __VA_ARGS__; }

using namespace TheatreFile;

void ArrayMesh::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);
    if(data.get_variable(mModelFilepath, "File", "Model") == OK)
        { LoadModelFile(mModelFilepath); }

    data.get_variable(mMaterialID, "Material");
}

Shared<ThingData> ArrayMesh::GetVariables() const
{
    auto data{Super::GetVariables()};
    data->set_variable(mModelFilepath, "Model");
    data->set_variable(mMaterialID, "Material");
    return data;
}

Shared<ArrayMesh> ArrayMesh::CreateFromMemory(const uchar* inData, size_t inSize, ModelFileType inType)
{
    auto output{DCast<ArrayMesh>(ThingFactory::MakeThing(ThingType::ArrayMesh, "Untitled_ArrayMesh"))};
    print_error_enum(output->LoadModelData(inData, inSize, inType));
    return output;
}

Shared<ArrayMesh> ArrayMesh::CreateFromFile(Sarg inFilePath)
{
    auto output{DCast<ArrayMesh>(ThingFactory::MakeThing(ThingType::ArrayMesh, "Untitled_ArrayMesh"))};
    print_error_enum(output->LoadModelFile(inFilePath));
    return output;
}

#pragma message("TODO: combine duplicate code")
Error ArrayMesh::LoadModelData(const uchar* inData, size_t inSize, ModelFileType inType)
{ return CreateMeshData({inData, inSize, sModelTypeToFileType(inType)}); }

Error ArrayMesh::LoadModelFile(Sarg inFilePath)
{ return CreateMeshData({inFilePath}); }

void ArrayMesh::AddSurface(PrimitiveType inType, Farg<MeshData_t> inData, Farg<Indices_t> inIndices)
{
    MeshData_t _mutable_mesh_data{};
    if(inData.empty() or inData[0].empty())
        { print_error("No vertex positions in ArrayMesh surface data"); return; }
    else if(inData.size() < ARRAY_FORMAT_MAX)
        { _mutable_mesh_data = inData; }

    Farg<MeshData_t> _mesh_data{(_mutable_mesh_data.empty()) ? inData : _mutable_mesh_data};
    auto& _surface{mSurfaces.emplace_back(inType, VertexArray::Create(), ID::Invalid)};

    for(int i{0}; i < ARRAY_FORMAT_MAX; ++i)
    {
        IBuffer::Element::Type _element_type{IBuffer::Element::Type::Float3};
        size_t _vertices_count{_mesh_data[0].size()};
        float _fill_value{0.0f};
        switch(i)
        {
        case Mesh::ARRAY_FORMAT_UV:
            _element_type = IBuffer::Element::Type::Float2;
            _vertices_count = _vertices_count - (_vertices_count / 3);
            break;
        case Mesh::ARRAY_FORMAT_COLORS:
            _fill_value = 1.0f;
            [[fallthrough]];
        default:
            break;
        }
        if(i >= _mesh_data.size())
            { _mutable_mesh_data[i] = std::vector<float>(_vertices_count, _fill_value); }
        auto vbo{VertexBuffer::Create(inData[i].data(), inData[i].size())};
        vbo->SetLayout({{_element_type, EnumRegistry::GetEnumName<ArrayFormat>(i)}});
        _surface.vertex_array->AddVertexBuffer(vbo);
    }

    std::vector<uint> _indices(inData[ARRAY_FORMAT_VERTEX].size() / 3);
    // https://stackoverflow.com/a/17694667
    std::iota(_indices.begin(), _indices.end(), 0);
    _surface.vertex_array->SetIndexBuffer(IndexBuffer::Create(_indices.data(), _indices.size()));
}

void ArrayMesh::ClearSurfaces()
{
    mSurfaces.clear();
}

void ArrayMesh::RemoveSurface(int inIndex)
{
    ASSERT_SURFACE_INDEX(inIndex)
    mSurfaces.erase(mSurfaces.cbegin() + inIndex);
}

int ArrayMesh::SurfaceCount() const
{
    return static_cast<int>(mSurfaces.size());
}

Mesh::PrimitiveType ArrayMesh::SurfaceGetPrimitive(int inIndex) const
{
    ASSERT_SURFACE_INDEX(inIndex, PRIMITIVE_MAX)
    return mSurfaces.at(inIndex).primitive;
}

Shared<VertexArray> ArrayMesh::SurfaceGetVertexArray(int inIndex) const
{
    ASSERT_SURFACE_INDEX(inIndex, nullptr)
    return mSurfaces.at(inIndex).vertex_array;
}

ID ArrayMesh::SurfaceGetMaterialID(int inIndex) const
{
    ASSERT_SURFACE_INDEX(inIndex, ID::Invalid)
    return mSurfaces.at(inIndex).material_id;
}

bool ArrayMesh::LoadedFromFile() const
{ return not mModelFilepath.empty(); }

Sarg ArrayMesh::ModelFilePath() const
{ return mModelFilepath; }

FileType ArrayMesh::sModelTypeToFileType(ModelFileType inType)
{
    switch(inType)
    {
    case ArrayMesh::MODEL_OBJ:
        return FileType::model_OBJ;
    default:
        print_warning("Invalid/unknown model file type encountered");
        return FileType::Unknown;
    }
}

Error ArrayMesh::CreateMeshData(Farg<FileData> inModelFile)
{
    if(not inModelFile.status())
        { return inModelFile.status(); }

    switch(inModelFile.file_type())
    {
    case FileType::model_OBJ:
        return CreateMeshFromOBJ(inModelFile);
    case FileType::Unknown:
    default:
        print_error("Data of unknown file-type cannot be buffered");
        return ERR_INVALID_TYPE;
    }
    return OK;
}

Error ArrayMesh::CreateMeshFromOBJ(Farg<FileData> inModelFile)
{
    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if(not reader.ParseFromString(inModelFile.raw_data_str(), "", reader_config))
    {
        if(!reader.Error().empty())
            { print_error("TinyObjReader Error: '{}'", reader.Error()); }
        return ERR_FILE_READ;
    }

    if(not reader.Warning().empty())
        { print_warning("TinyObjReader Warning: '{}'", reader.Warning()); }

    auto& attrib{reader.GetAttrib()};
    auto& shapes{reader.GetShapes()};

    std::vector<float> _positions{}, _colors{}, _normals{}, _uvs{};
    const std::vector<float> _empty_color{1.0f, 1.0f, 1.0f},
        _empty_normal{0.0f, 0.0f, 0.0f},
        _empty_uv{0.0f, 0.0f};

    // This looping code is from https://github.com/tinyobjloader/tinyobjloader
    // Loop over shapes (Shapes are full meshes in the OBJ (there can be multiple))
    for(size_t s = 0; s < shapes.size(); ++s)
    {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
        {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for(size_t v = 0; v < fv; ++v)
            {
                // access to vertex
                tinyobj::index_t idx{shapes[s].mesh.indices[index_offset + v]};
                _positions.push_back(attrib.vertices[3*size_t(idx.vertex_index)+0]);
                _positions.push_back(attrib.vertices[3*size_t(idx.vertex_index)+1]);
                _positions.push_back(attrib.vertices[3*size_t(idx.vertex_index)+2]);

                std::vector<float> _color{_empty_color}, _normal{_empty_normal}, _uv{_empty_uv};

                if(not attrib.colors.empty())
                {
                    _color[0] = attrib.colors[3*size_t(idx.vertex_index)+0];
                    _color[1] = attrib.colors[3*size_t(idx.vertex_index)+1];
                    _color[2] = attrib.colors[3*size_t(idx.vertex_index)+2];
                }

                // Check if `normal_index` is zero or positive. negative = no normal data
                if(idx.normal_index >= 0)
                {
                    _normal[0] = attrib.normals[3*size_t(idx.normal_index)+0];
                    _normal[1] = attrib.normals[3*size_t(idx.normal_index)+1];
                    _normal[2] = attrib.normals[3*size_t(idx.normal_index)+2];
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if(idx.texcoord_index >= 0)
                {
                    _uv[0] = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                    _uv[1] = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
                }
                _colors.insert(_colors.end(), _color.begin(), _color.end());
                _normals.insert(_normals.end(), _normal.begin(), _normal.end());
                _uvs.insert(_uvs.end(), _uv.begin(), _uv.end());
            }
            index_offset += fv;
        }
        AddSurface(PRIMITIVE_TRIANGLES, {_positions, _colors, _normals, _uvs});
    }

    return OK;
}
