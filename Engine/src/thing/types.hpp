#ifndef THING_TYPES_H
#define THING_TYPES_H

#include "core/id.hpp"

#define TYPE_DEF(TYPE) inline const TTID TYPE{#TYPE};

struct TTID : public PID { using PID::PID; };

namespace ThingType
{
    TYPE_DEF(Thing)
    TYPE_DEF(Resource)
    TYPE_DEF(Mesh)
    TYPE_DEF(Texture)
    TYPE_DEF(Material)
    TYPE_DEF(Thinker)
    TYPE_DEF(Viewport)
    TYPE_DEF(Actor3D)
    TYPE_DEF(Collider)
    TYPE_DEF(MeshInstance3D)
    TYPE_DEF(PointLight)
    TYPE_DEF(SpotLight)
    TYPE_DEF(DirectionalLight)
    TYPE_DEF(Camera3D)
    TYPE_DEF(NostalgiaPlayer)
    TYPE_DEF(Actor2D)
}

#undef TYPE_DEF
#endif // THING_TYPES_H
