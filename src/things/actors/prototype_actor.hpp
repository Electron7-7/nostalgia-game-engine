#ifndef PROTOTYPE_ACTOR_H
#define PROTOTYPE_ACTOR_H

#include "../actor.hpp"

#include "resources/resource.hpp"

class PrototypeActor : public Actor
{
public:
    tid_t GetMeshInstance() const;
    bool HasMeshInstance() const;

    void SetMeshInstance(rid_t MeshInstanceID);

protected:
    friend class TheatreManager;
    rid_t m_MeshInstanceID = Resource::NoRID;
};

#endif // PROTOTYPE_ACTOR_H
