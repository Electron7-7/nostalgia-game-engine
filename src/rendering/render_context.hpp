#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

// RenderContext will be a container for holding the different abstract data structures (i.e: MeshData). The abstract
// structures will be classes that can account for any data type required by future graphics APIs. The goal here is to
// keep these top-level classes future proof.

#include "things/id.hpp"

#include <glm/glm.hpp>

struct RenderContext
{
public:
    glm::mat4 GetModelMatrix()      const { return model_matrix;             }
    id_t GetShaderID()              const { return shader_id;                }
    int GetPointLightsCount()       const { return point_lights_count;       }
    int GetSpotLightsCount()        const { return spot_lights_count;        }
    int GetDirectionalLightsCount() const { return directional_lights_count; }

    void SetModelMatrix(const glm::mat4& ModelMatrix)           { model_matrix = ModelMatrix;           }
    void SetShaderID(id_t ShaderID)                             { shader_id = ShaderID;                 }
    void SetPointLightsCount(int Count)                         { point_lights_count = Count;           }
    void SetSpotLightsCount(int Count)                          { spot_lights_count = Count;            }
    void SetDirectionalLightsCount(int Count)                   { directional_lights_count = Count;     }

private:
    glm::mat4 model_matrix;
    id_t shader_id = 0;
    int point_lights_count = 0;
    int spot_lights_count = 0;
    int directional_lights_count = 0;
};

#endif // RENDER_CONTEXT_H
