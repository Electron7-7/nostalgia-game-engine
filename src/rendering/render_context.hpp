#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

// RenderContext will be a container for holding the different abstract data structures (i.e: MeshData). The abstract
// structures will be classes that can account for any data type required by future graphics APIs. The goal here is to
// keep these top-level classes future proof.

#include <glm/glm.hpp>

struct RenderContext
{
public:
    glm::mat4 GetProjectionMatrix() const { return projection_matrix;        }
    glm::mat4 GetViewMatrix()       const { return view_matrix;              }
    glm::mat4 GetModelMatrix()      const { return model_matrix;             }
    glm::vec3 GetViewPosition()     const { return view_position;            }
    int GetMeshID()                 const { return mesh_id;                  }
    unsigned int GetShaderID()      const { return shader_id;                }
    int GetPointLightsCount()       const { return point_lights_count;       }
    int GetSpotLightsCount()        const { return spot_lights_count;        }
    int GetDirectionalLightsCount() const { return directional_lights_count; }

    void SetProjectionMatrix(glm::mat4 ProjectionMatrix) { projection_matrix = ProjectionMatrix; }
    void SetViewMatrix(glm::mat4 ViewMatrix)             { view_matrix = ViewMatrix;             }
    void SetModelMatrix(glm::mat4 ModelMatrix)           { model_matrix = ModelMatrix;           }
    void SetViewPosition(glm::vec3 ViewPosition)         { view_position = ViewPosition;         }
    void SetMeshID(int MeshID)                           { mesh_id = MeshID;                     }
    void SetShaderID(unsigned int ShaderID)              { shader_id = ShaderID;                 }
    void SetPointLightsCount(int Count)                  { point_lights_count = Count;           }
    void SetSpotLightsCount(int Count)                   { spot_lights_count = Count;            }
    void SetDirectionalLightsCount(int Count)            { directional_lights_count = Count;     }

private:
    glm::mat4 projection_matrix;
    glm::mat4 view_matrix;
    glm::mat4 model_matrix;
    glm::vec3 view_position;
    int mesh_id = -1;
    unsigned int shader_id = 0;
    int point_lights_count = 0;
    int spot_lights_count = 0;
    int directional_lights_count = 0;
};

#endif // RENDER_CONTEXT_H
