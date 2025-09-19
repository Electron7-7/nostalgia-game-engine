#include "prototype_actor.hpp"
#include "../things.hpp"
#include "../resources/complex/mesh_instance.hpp"
#include "rendering/shader_interfaces/shader_interface.hpp"

#include <glm/ext.hpp>

void PrototypeActor::SetupVariables(const data_t& data)
{
    Actor::SetupVariables(data);

    data.GetTheatreRef(m_MeshInstanceID, "MeshInstance");
}

RenderCommand PrototypeActor::GetRenderCommand()
{
    RenderContext render_context;

    auto mesh_instance = g_GetThing<MeshInstance>(m_MeshInstanceID);
    auto material = g_GetThing<Material>(mesh_instance->GetMaterialID());

    glm::mat4 model_matrix(1.0f);
    model_matrix  = glm::translate(glm::mat4(1.0f), Origin());
    model_matrix *= glm::toMat4(Quaternion());
    model_matrix *= glm::scale(Scale());

    render_context.SetShaderID(Shaders::BlinnPhong);
    render_context.SetModelMatrix(model_matrix);

    return RenderCommand(render_context, mesh_instance->GetMeshID(), material);
}

id_t PrototypeActor::GetMeshInstance() const
{ return m_MeshInstanceID; }

void PrototypeActor::SetMeshInstance(id_t mesh_instance_id)
{ m_MeshInstanceID = mesh_instance_id; }
