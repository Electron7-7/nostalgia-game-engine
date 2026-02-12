#ifndef NOSTALGIA_GOGGLES_THING_TYPES_H
#define NOSTALGIA_GOGGLES_THING_TYPES_H

#include <Nostalgia/Nostalgia.hpp>

#define TYPE_ID(TYPE) inline static const PID TYPE{#TYPE};

namespace TypeIDs
{
    TYPE_ID(EditorPlayer3D)
}

#endif // NOSTALGIA_GOGGLES_THING_TYPES_H
