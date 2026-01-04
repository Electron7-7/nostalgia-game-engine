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
        Viewport            {"Viewport"},
        Collider            {"Collider"},
        Material            {"Material"},
        MeshInstance3D      {"MeshInstance3D"},
        Actor3D             {"Actor3D"},
        Actor2D             {"Actor2D"},
        PointLight          {"PointLight"},
        SpotLight           {"SpotLight"},
        DirectionalLight    {"DirectionalLight"},
        Camera3D            {"Camera3D"},
        NostalgiaPlayer     {"NostalgiaPlayer"};
}

#endif // THING_TYPES_H
