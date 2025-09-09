#ifndef MESH_H
#define MESH_H

#include "resources/resource.hpp"
#include "rendering/vertex.hpp"
#include "math/unsigned_int_vector.hpp"
#include "safe_return.hpp"

#include <glm/glm.hpp>
#include <vector>

enum class ModelType
{
    Unknown,
    OBJ
};

struct Mesh : public Resource
{
public:
    typedef unsigned int Index;

    static Mesh Error;
    static Mesh Ramiel;

    Mesh();

    void LoadModelFile(const std::string& FilePath, ModelType FileType = ModelType::Unknown);
    void LoadModelData(const std::string& FileData, ModelType FileType);

    SafeStatus CreateMeshData();

    void clear();

    const std::vector<Vertex>& GetVertices() const;
    const std::vector<Index>& GetIndices() const;

    static void GetVertexData(std::vector<Vertex>* Vertices, std::vector<float>* VertexData);

private:
    friend class TheatreManager;
    Mesh(const std::string& EngineModelData, ModelType FileType);

    ResourceStatus m_MeshDataStatus = ResourceStatus::NOT_PROCESSED;

    ModelType m_FileType = ModelType::Unknown;
    std::string m_MeshFile = "";
    std::string m_MeshFileData = "";

    std::vector<Vertex> m_Vertices = {};
    std::vector<Index>  m_Indices  = {};

    SafeStatus try_CreateOBJMesh();

    void AddVertices(const std::vector<Vertex>& Vertices);
    void AddIndices(const std::vector<Index>& Indices);
    void AddIndices(const std::vector<uintvec3>& Faces);

    void AddVertex(Vertex NewVertex);
    void AddIndex(Index NewIndex);
    void AddFace(uintvec3 NewFaceIndices);
};

#endif // MESH_H
