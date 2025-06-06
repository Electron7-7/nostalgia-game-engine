#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

// RenderContext will be a container for holding the different abstract data structures (i.e: MeshData). The abstract
// structures will be classes that can account for any data type required by future graphics APIs. The goal here is to
// keep these top-level classes future proof.

#include <glm/glm.hpp>
#include "mesh.hpp"

struct RenderContext
{
public:
    glm::mat4 GetProjectionMatrix() const { return projection_matrix; }
    glm::mat4 GetViewMatrix()       const { return view_matrix;       }
    glm::mat4 GetModelMatrix()      const { return model_matrix;      }
    glm::vec3 GetViewPosition()     const { return view_position;     }
    Mesh      GetMesh()             const { return mesh_to_render;    }

    void SetProjectionMatrix(glm::mat4 ProjectionMatrix) { projection_matrix = ProjectionMatrix; }
    void SetViewMatrix(glm::mat4 ViewMatrix)             { view_matrix = ViewMatrix;             }
    void SetModelMatrix(glm::mat4 ModelMatrix)           { model_matrix = ModelMatrix;           }
    void SetViewPosition(glm::vec3 ViewPosition)         { view_position = ViewPosition;         }
    void SetMesh(const Mesh& MeshToRender)               { mesh_to_render = MeshToRender;        }

private:
    glm::mat4 projection_matrix;
    glm::mat4 view_matrix;
    glm::mat4 model_matrix;
    glm::vec3 view_position;
    Mesh mesh_to_render;
};

#endif // RENDER_CONTEXT_H
