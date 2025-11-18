#ifndef PHYSICS_ENUMS_H
#define PHYSICS_ENUMS_H

#include "core/penum_t.hpp"

namespace PhysicsBodyShape
{
    constexpr static penum_t Box      { 0, "Box"      };
    constexpr static penum_t Sphere   { 1, "Sphere"   };
    constexpr static penum_t Capsule  { 2, "Capsule"  };
    constexpr static penum_t Cylinder { 3, "Cylinder" };
}

namespace PhysicsBodyMotion
{
    constexpr static penum_t Dynamic   { 0, "Dynamic"   };
    constexpr static penum_t Kinematic { 1, "Kinematic" };
    constexpr static penum_t Static    { 2, "Static"    };
}

#endif // PHYSICS_ENUMS_H
