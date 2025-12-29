#ifndef THING_TYPES_H
#define THING_TYPES_H

#include "core/id.hpp"

struct TTID : public PID { using PID::PID; };

namespace ThingType
{
    inline const TTID Thing {"Thing"},
        Thinker             {"Thinker"},
        Resource            {"Resource"},
        Mesh                {"Mesh"},
        Texture             {"Texture"},
        Font                {"Font"},
        Device              {"Device"},
        Collider            {"Collider"},
        Material            {"Material"},
        MeshInstance        {"MeshInstance"},
        Actor               {"Actor"},
        PointLight          {"PointLight"},
        SpotLight           {"SpotLight"},
        DirectionalLight    {"DirectionalLight"},
        Camera3D            {"Camera3D"},
        NostalgiaPlayer     {"NostalgiaPlayer"};
}

#endif // THING_TYPES_H
