#ifndef PHYSICS_FRAMEWORK_H
#define PHYSICS_FRAMEWORK_H

// See: github.com/jrouwe/JoltPhysics/blob/master/UnitTests/UnitTestFramework.h

// Note: I use angle includes here to suppress clang's "unused header" warnings
#include <Jolt/Jolt.h>
#include <Jolt/Core/Atomics.h>
#include <Jolt/Math/DVec3.h>
#include <Jolt/Math/Float2.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <physics/enums.hpp>

// Disable common warnings
JPH_SUPPRESS_WARNINGS
JPH_CLANG_SUPPRESS_WARNING("-Wheader-hygiene")
#ifdef JPH_DOUBLE_PRECISION
JPH_CLANG_SUPPRESS_WARNING("-Wdouble-promotion")
#endif // JPH_DOUBLE_PRECISION

#endif // PHYSICS_FRAMEWORK_H
