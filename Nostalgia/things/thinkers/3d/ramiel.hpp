#ifndef RAMIEL_H
#define RAMIEL_H

#include <Nostalgia/things/thinkers/3d/actor_3d.hpp>

class Ramiel : public Actor3D
{
public:
    SET_SUPER(Actor3D)
    DEFINE_TYPEID(Ramiel, RamielType)
    virtual SIG_READY override;
    virtual SIG_SET_VARIABLES override;
    virtual SIG_GET_VARIABLES override;
    virtual SIG_TICK override;

private:
    float mSpeed{1.0f};
    glm::vec3 mDirection{0.0f, 0.0f, 1.0f};
};

#endif // RAMIEL_H
