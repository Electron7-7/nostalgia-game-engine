#ifndef NOSTALGIA_PLAYER_H
#define NOSTALGIA_PLAYER_H

#include "things/actor.hpp"

class CameraProperty; // Forward Declaration

// Actor type
class NostalgiaPlayer : public Actor
{
public:
    // FIXME: Put this in a .cpp file
    CameraProperty* GetCameraProperty() const { return player_CameraProperty; }

private:
    CameraProperty* player_CameraProperty;
};

#endif // NOSTALGIA_PLAYER_H
