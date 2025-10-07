#ifndef ACTOR_H
#define ACTOR_H

#include "../thing.hpp"
#include "world/transform_3d.hpp"

class Actor : public Thing, public Transform3D
{
public:
    virtual void SetupVariables(const ThingData&);
    virtual bool Initialize() { return true; }
    virtual void Destroy() {}

    virtual void Update() {}
    virtual void Tick() {}

    ID GetMeshInstanceID() const;
    void SetMeshInstanceID(ID MeshInstanceID);

    bool mVisible = true;
    bool mWireframe = false;

protected:
    ID mMeshInstanceID = ID::None;
};

#endif // ACTOR_H
