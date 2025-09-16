#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "render_context.hpp"
#include "things/fwd.hpp"

#include <memory>

// FIXME: RenderCommand & RenderContext might be overkill and if RenderCommand doesn't really do anything but hold a RenderContext, I'd like to just replace RenderContext with RenderCommand
struct RenderCommand
{
public:
    RenderCommand();
    RenderCommand(RenderContext NewRenderContext, id_t MeshID, std::shared_ptr<Material> NewMaterial);

    const bool& IsSafe() const;
    const RenderContext& GetRenderContext() const;
    const glm::vec3& ViewPosition() const;
    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;
    id_t GetMeshID() const;
    std::shared_ptr<Material> GetMaterial() const;

private:
    bool m_IsSafe = false;

    RenderContext m_RenderContext;
    id_t m_MeshID = ID::None;
    std::shared_ptr<Material> m_Material;
};

#endif // RENDER_COMMAND_H
