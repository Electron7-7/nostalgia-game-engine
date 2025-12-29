#ifndef PHYSICS_FWD_H
#define PHYSICS_FWD_H

/////////////////////////
// Begin Jolt Boilerplate
// See: github.com/jrouwe/JoltPhysics/blob/master/UnitTests/UnitTestFramework.h
// Note: I use angle includes here to suppress clang's "unused header" warnings
#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Core/Atomics.h>
#include <Jolt/Math/DVec3.h>
#include <Jolt/Math/Float2.h>
// Disable common warnings
JPH_SUPPRESS_WARNINGS
JPH_CLANG_SUPPRESS_WARNING("-Wheader-hygiene")
#ifdef JPH_DOUBLE_PRECISION
JPH_CLANG_SUPPRESS_WARNING("-Wdouble-promotion")
#endif // JPH_DOUBLE_PRECISION
// End Jolt Boilerplate
///////////////////////

// Some JPH namespace forward declarations
namespace JPH
{
    class BodyInterface;
    class BodyID;
    class TempAllocator;
    class JobSystem;
    class PhysicsSystem;
    class Float2;
}

// Collider enums
enum class PhysicsBodyShape  : unsigned short;
enum class PhysicsBodyMotion : unsigned short;

#endif // PHYSICS_FWD_H
