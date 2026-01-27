#ifndef ACTOR_3D_H
#define ACTOR_3D_H

// Similar to Godot's `Node3D`
class Actor3D : public Thinker
{
public:
    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

    virtual void SetPosition(Farg<glm::vec3>);
    virtual void SetQuaternion(Farg<glm::quat>);
    virtual void SetRotation(Farg<glm::vec3>);
    virtual void SetRotationDegrees(Farg<glm::vec3>);
    virtual void SetScale(Farg<glm::vec3>);

    Farg<glm::vec3> Position() const;
    Farg<glm::quat> Quaternion() const;
    Farg<glm::vec3> Rotation() const;
    Farg<glm::vec3> RotationDegrees() const;
    Farg<glm::vec3> Scale() const;

    glm::vec3 GlobalPosition() const;
    glm::vec3 GlobalRotation() const;
    glm::vec3 GlobalRotationDegrees() const;
    glm::vec3 GlobalScale() const;

    void SetGlobalPosition(Farg<glm::vec3>);
    void SetGlobalRotation(Farg<glm::vec3>);
    void SetGlobalRotationDegrees(Farg<glm::vec3>);
    void SetGlobalScale(Farg<glm::vec3>);

protected:
    glm::vec3 mPosition{0.0f};
    glm::quat mQuaternion{};
    glm::vec3 mEulerRotationRadians{0.0f};
    glm::vec3 mEulerRotationDegrees{0.0f};
    glm::vec3 mScale{1.0f};
};

#endif // ACTOR_3D_H
