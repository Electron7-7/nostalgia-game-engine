#ifndef NOSTALGIA_PLAYER_3D_H
#define NOSTALGIA_PLAYER_3D_H

#include <Nostalgia/theatre/things/thinkers/3d/actor_3d.hpp>

class NostalgiaPlayer3D : public Actor3D
{
public:
    READY_OVERRIDE
    GET_VARIABLES_OVERRIDE
    SET_VARIABLES_OVERRIDE

protected:
    ID mCameraID{};
    bool mCreateDefaultCamera{true};
};

#endif // NOSTALGIA_PLAYER_3D_H
