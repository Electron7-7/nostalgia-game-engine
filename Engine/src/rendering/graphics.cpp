#define DO_NOT_BUILD
#ifndef DO_NOT_BUILD
#include "graphics.hpp"
#include "managers/backend_manager.hpp"
#include "renderer/vertex.hpp"
#include "filesystem/file_data.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include "thirdparty/TinyOBJLoader/tiny_obj_loader.h"

#include <list>

static bool s_CreateOBJMesh(std::vector<float>&, std::vector<unsigned int>&, const FileData&);

void GraphicsBackend::BufferRenderCommand(const RenderCommand& command)
{ mRenderCommands.push_back(command); }

void GraphicsBackend::RenderStoredCommands()
{
    if(mRenderCommands.empty())
        { return; }
    g_pBackendManager->Windowing()->ClearBuffer(glm::vec4(0.29f, 0.34f, 0.26f, 1.0f));
    std::list<RenderCommand> current_commands{mRenderCommands.cbegin(), mRenderCommands.cend()};
    mRenderCommands.clear();
    do
    {
        RenderSingleCommand(current_commands.back());
        current_commands.pop_back();
    }
    while(current_commands.size() > 0);
}

bool GraphicsBackend::UseViewport(const ID& id)
{
    if(!IsViewportAt(id))
        { return false; }
    else if(mCurrentViewport == id)
        { return true; }
    mLastUsedViewport = mCurrentViewport;
    mCurrentViewport  = id;
    UpdateViewport(mViewports[mCurrentViewport]);
    return true;
}

const ID& GraphicsBackend::CurrentViewportID()
{ return mCurrentViewport; }

const Viewport& GraphicsBackend::GetViewport(const ID& id)
{
    if(!IsViewportAt(id))
    {
        print_warning("GraphicsBackend::GetViewport - invalid id {}, the window viewport will be returned instead", id);
        return WindowViewport();
    }
    return mViewports.at(id);
}

const Viewport& GraphicsBackend::WindowViewport()
{ return mViewports.at(ViewportIDs::Window); }

const Viewport& GraphicsBackend::CurrentViewport()
{
    if(!IsViewportAt(mCurrentViewport))
    {
        print_warning("GraphicsBackend::GetCurrentViewport - current viewport id {} is invalid; setting it to the window viewport", mCurrentViewport);
        mCurrentViewport = ViewportIDs::Window;
    }
    return mViewports[mCurrentViewport];
}

bool GraphicsBackend::SetViewport(const ID& id, const Viewport& viewport)
{
    if(id == ViewportIDs::Window)
        { return print_warning("GraphicsBackend::SetViewport - cannot change the window viewport with this function, please use `GraphicsBackend::SetWindowViewport` instead"); }
    else if(!IsViewportAt(id))
        { return false; }
    mViewports[id] = viewport;
    return true;
}

void GraphicsBackend::SetWindowViewport(const Viewport& viewport)
{ mViewports.at(ViewportIDs::Window) = viewport; }

bool GraphicsBackend::SetCurrentViewport(const Viewport& viewport)
{ return SetViewport(mCurrentViewport, viewport); }

bool GraphicsBackend::IsViewportAt(const ID& id)
{ return mViewports.contains(id); }

id_t GraphicsBackend::PushViewport(const Viewport& viewport)
{
    id_t new_id{ID::front};
    do { new_id = ID::Generate(); } while ( mViewports.contains(new_id) );
    mViewports[new_id] = viewport;
    return new_id;
}

bool GraphicsBackend::PopViewport(const ID& id)
{
    if(id == ViewportIDs::Window)
        { return print_error("GraphicsBackend::PopViewport - cannot remove the window viewport"); }
    return mViewports.erase(id) > 0;
}

bool GraphicsBackend::CreateMeshData(std::vector<float>& vertices, std::vector<unsigned int>& indices, const FileData& data)
{
    if(data.Status() == DataStatus::FAILED)
        { return false; }

    switch(data.Type())
    {
    case FileType::model_OBJ:
        return s_CreateOBJMesh(vertices, indices, data);
        break;
    case FileType::Unknown:
    default:
        print_warning("GraphicsBackend::CreateMeshData - Data of unknown file-type cannot be buffered");
        return false;
    }
    return true;
}

// Taken from GraphX
bool s_CreateOBJMesh(std::vector<float>& vertices, std::vector<unsigned int>& indices, const FileData& data)
{
    tinyobj::ObjReaderConfig reader_config;
    tinyobj::ObjReader reader;

    if(!reader.ParseFromString(data.String(), "", reader_config))
    {
        if(!reader.Error().empty())
            { return print_error("GraphicsBackend::try_CreateOBJMesh - TinyObjReader Error: '{}'", reader.Error()); }
    }

    if(!reader.Warning().empty())
        { print_warning("GraphicsBackend::try_CreateOBJMesh - TinyObjReader Warning: '{}'", reader.Warning()); }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

    unsigned int vertex_count = 0;
    Vertex temp_vertex;

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
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

                temp_vertex.SetPosition(glm::vec3((float)vx, (float)vy, (float)vz));

                // Check if `normal_index` is zero or positive. negative = no normal data
                if(idx.normal_index >= 0)
                {
                    tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                    tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                    tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];

                    temp_vertex.SetNormal(glm::vec3((float)nx, (float)ny, (float)nz));
                }
                else
                    { temp_vertex.SetNormal(glm::vec3(0.0f)); }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if(idx.texcoord_index >= 0)
                {
                    tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                    tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];

                    temp_vertex.SetUV(glm::vec2((float)tx, (float)ty));
                }
                else
                    { temp_vertex.SetUV(glm::vec2(0.0f)); }

                if(idx.texcoord_index >= 0)
                {
                    tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                    tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                    tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];

                    temp_vertex.SetColor(glm::vec3((float)red, (float)green, (float)blue));
                }
                else
                    { temp_vertex.SetColor(glm::vec3(1.0f)); }

                temp_vertex.GetVertexData(vertices);
                indices.push_back(vertex_count++);
            }
            index_offset += fv;
        }
    }
    return true;
}
#endif // DO_NOT_BUILD
