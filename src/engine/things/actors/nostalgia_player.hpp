#ifndef NOSTALGIA_PLAYER_H
#define NOSTALGIA_PLAYER_H

#include "common/macros.hpp"
#include "engine/things/actor.hpp"

FORWARD_DECLARE(class CameraProperty;)

// Actor type
class NostalgiaPlayerActor : public Actor
{
public:
    // FIXME: Put this in a .cpp file
    CameraProperty* GetCameraProperty() const { return player_CameraProperty; }

private:
    CameraProperty* player_CameraProperty;
};

#endif // NOSTALGIA_PLAYER_H
