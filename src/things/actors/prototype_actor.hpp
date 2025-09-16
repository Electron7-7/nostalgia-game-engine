#ifndef PROTOTYPE_ACTOR_H
#define PROTOTYPE_ACTOR_H

#include "actor.hpp"

extern float g_DebugMeshPosition[3];

class PrototypeActor : public Actor
{
public:
    virtual void SetupVariables(const data_t&);
    virtual RenderCommand GetRenderCommand();

    id_t GetMeshInstance() const;
    void SetMeshInstance(id_t MeshInstanceID);

protected:
    id_t m_MeshInstanceID = ID::None;
};

#endif // PROTOTYPE_ACTOR_H
