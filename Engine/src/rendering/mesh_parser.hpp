#ifndef MESH_PARSER_H
#define MESH_PARSER_H

#define FWD_DCL
#   include "filesystem/file_data.hpp"
#undef  FWD_DCL

#include "core/farg.hpp"

#include <vector>
#include <sys/types.h>

namespace MeshParser
{
    bool CreateMeshData(std::vector<float>& ioVertices, std::vector<uint>& ioIndices, Farg<FileData> inData);
    bool CreateOBJMesh(std::vector<float>& ioVertices, std::vector<uint>& ioIndices, Farg<FileData> inData);
}

#endif // MESH_PARSER_H
