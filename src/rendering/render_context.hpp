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

private:
    friend class RenderManager;

    glm::mat4 projection_matrix;
    glm::mat4 view_matrix;
    glm::mat4 model_matrix;
    glm::vec3 view_position;
};

#endif // RENDER_CONTEXT_H