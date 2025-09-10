#ifndef PROTOTYPE_ACTOR_H
#define PROTOTYPE_ACTOR_H

#include "actor.hpp"

class PrototypeActor : public Actor
{
public:

    id_t GetMeshInstance() const;
    bool HasMeshInstance() const;

    void SetMeshInstance(id_t MeshInstanceID);

protected:
    friend class TheatreManager;
    id_t m_MeshInstanceID = NoID;
};

#endif // PROTOTYPE_ACTOR_H
