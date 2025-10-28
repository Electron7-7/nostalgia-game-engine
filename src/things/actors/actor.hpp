#ifndef ACTOR_H
#define ACTOR_H

#include "../thing.hpp"
#include "interfaces/transform_3d.hpp"

class Actor : public Thing, public Transform3D
{
public:
    virtual void Ready();
    virtual void Tick();
    virtual void Update() {}
    virtual void Input(const InputEvent&) {}
    virtual void SetVariables(const ThingData&);
    virtual ThingData GetVariables() const;

    ID MeshInstanceID() const;
    void MeshInstanceID(ID MeshInstanceID);

    ID ColliderID() const;
    void ColliderID(ID UID);

    bool mVisible{true};
    bool mWireframe{false};

    // Off by default (alpha == 0.0f)
    glm::vec4 mDebugHighlight{1.0f, 0.2f, 0.9f, 0.0f};

protected:
    ID mMeshInstanceID{};
    ID mColliderID{};
};

#endif // ACTOR_H
