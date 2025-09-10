#ifndef NOSTALGIA_PLAYER_H
#define NOSTALGIA_PLAYER_H

#include "actor.hpp"
#include "camera_3d.hpp"

// Actor type
class NostalgiaPlayer : public Actor
{
public:
    virtual void SetupVariables(const data_t&);
    bool Initialize();

    void SetOrigin(const glm::vec3&);
    void SetRotation(const glm::quat&);

    Camera3D* GetCamera();

    static NostalgiaPlayer Empty;

private:
    id_t m_CameraID = NoID;
};

#endif // NOSTALGIA_PLAYER_H
