#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

#include <glm/glm.hpp>

struct RenderContext
{
public:
    glm::mat4 GetProjectionMatrix() const { return projection_matrix; }
    glm::mat4 GetViewMatrix()       const { return view_matrix;       }
    glm::mat4 GetModelMatrix()      const { return model_matrix;      }
    glm::vec3 GetViewPosition()     const { return view_position;     }

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
};

#endif // RENDER_CONTEXT_H
