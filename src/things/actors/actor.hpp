#ifndef ACTOR_H
#define ACTOR_H

#include "../thing.hpp"
#include "rendering/render_command.hpp"
#include "types/setup_variables.hpp"
#include "world/position_3d.hpp"

class Actor : public Thing, public Position3D, public DoSetupVariables
{
public:
    virtual void SetupVariables(const data_t&);
    virtual bool Initialize() { return true; }
    virtual void Destroy() {}

    virtual RenderCommand GetRenderCommand() { return RenderCommand(); }
    virtual void Update() {}
    virtual void Tick() {}
};

#endif // ACTOR_H
