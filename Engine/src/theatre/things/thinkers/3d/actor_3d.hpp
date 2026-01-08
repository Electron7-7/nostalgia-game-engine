#ifndef ACTOR_3D_H
#define ACTOR_3D_H

#include "theatre/things/thinkers/thinker.hpp"
#include <glm/vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

// Similar to Godot's `Node3D`
class Actor3D : public Thinker
{
public:
    virtual void Ready() override;
    virtual void Tick() override;
    virtual void Update() override {}
    virtual void Input(InputEvent*) override {}
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    virtual Farg<glm::vec3> Position() const;
    virtual Farg<glm::quat> Quaternion() const;
    virtual Farg<glm::vec3> Rotation() const;
    virtual Farg<glm::vec3> RotationDegrees() const;
    virtual Farg<glm::vec3> Scale() const;
    virtual bool Visible() const;

    virtual void SetPosition(Farg<glm::vec3>);
    virtual void SetQuaternion(Farg<glm::quat>);
    virtual void SetRotation(Farg<glm::vec3>);
    virtual void SetRotationDegrees(Farg<glm::vec3>);
    virtual void SetScale(Farg<glm::vec3>);
    virtual void SetVisible(bool);

    glm::vec4 mDebugHighlight{1.0f, 0.2f, 0.9f, 0.0f}; // Off by default (alpha == 0.0f)
    ID mDebugMeshInstanceID{};

protected:
    glm::vec3 mPosition{0.0f};
    glm::quat mQuaternion{};
    glm::vec3 mEulerRotationRadians{0.0f};
    glm::vec3 mEulerRotationDegrees{0.0f};
    glm::vec3 mScale{1.0f};

    bool mVisible{true};
};

#endif // ACTOR_3D_H
