#ifndef ACTOR_H
#define ACTOR_H

#include "../thing.hpp"
#include "world/transform_3d.hpp"

class Actor : public Thing, public Transform3D
{
public:
    virtual void Tick() {}
    virtual void Update() {}
    virtual void Input(const InputEvent&) {}
    virtual void SetVariables(const ThingData&);
    virtual ThingData GetVariables() const;

    ID GetMeshInstanceID() const;
    void SetMeshInstanceID(ID MeshInstanceID);

    bool mVisible = true;
    bool mWireframe = false;

protected:
    ID mMeshInstanceID = ID::None;
};

#endif // ACTOR_H
