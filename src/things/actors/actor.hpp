#ifndef ACTOR_H
#define ACTOR_H

#include "../thing.hpp"
#include "world/transform_3d.hpp"

class Actor : public Thing, public Transform3D
{
public:
    virtual void SetupVariables(const data_t&);
    virtual bool Initialize() { return true; }
    virtual void Destroy() {}

    virtual void Update() {}
    virtual void Tick() {}

    id_t GetMeshInstanceID() const;
    void SetMeshInstanceID(id_t MeshInstanceID);

    bool m_Visible = true;
    bool m_Wireframe = false;

protected:
    id_t m_MeshInstanceID = IDs::None;
};

#endif // ACTOR_H
