#ifndef NOSTALGIA_PLAYER_H
#define NOSTALGIA_PLAYER_H

#include <Nostalgia/fwd/things.hpp>
#include <Nostalgia/things/thinkers/thinker.hpp>

class NostalgiaPlayer : public Thinker
{
public:
    THING_CLASS(NostalgiaPlayer, Thinker)
    READY_OVERRIDE
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    Shared<Actor3D> GetActor3D() const;
    Shared<Actor2D> GetActor2D() const;

protected:
    Shared<Actor3D> m_pActor3D{nullptr};
    Shared<Actor2D> m_pActor2D{nullptr};
};

#endif // NOSTALGIA_PLAYER_H
