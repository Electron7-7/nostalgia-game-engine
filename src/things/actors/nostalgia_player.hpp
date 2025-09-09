#ifndef NOSTALGIA_PLAYER_H
#define NOSTALGIA_PLAYER_H

#include "../actor.hpp"
#include "camera_3d.hpp"

// Actor type
class NostalgiaPlayer : public Actor
{
public:
    void SetOrigin(const glm::vec3&);
    void SetRotation(const glm::quat&);

    Camera3D* GetCamera();

    static NostalgiaPlayer Empty;

private:
    tid_t m_CameraID;
};

#endif // NOSTALGIA_PLAYER_H
