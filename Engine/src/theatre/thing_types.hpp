#ifndef THING_TYPES_H
#define THING_TYPES_H

#include "core/id.hpp"

#define TYPE_DEF(TYPE) inline const PID TYPE{#TYPE};

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
    TYPE_DEF(Collider3D)
    TYPE_DEF(MeshInstance3D)
    TYPE_DEF(Light3D)
    TYPE_DEF(PointLight3D)
    TYPE_DEF(SpotLight3D)
    TYPE_DEF(DirectionalLight3D)
    TYPE_DEF(Camera3D)
    TYPE_DEF(NostalgiaPlayer3D)
    TYPE_DEF(Actor2D)
}

#undef TYPE_DEF
#endif // THING_TYPES_H
