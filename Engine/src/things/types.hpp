#ifndef THING_TYPES_H
#define THING_TYPES_H

#include "core/id.hpp"

// struct TTID : public PID { using PID::PID; };
using TTID = PID;

namespace ThingType
{
    const TTID Thing            {"Thing"};
    const TTID Thinker          {"Thinker"};
    const TTID Resource         {"Resource"};
    const TTID Mesh             {"Mesh"};
    const TTID Texture          {"Texture"};
    const TTID Font             {"Font"};
    const TTID Device           {"Device"};
    const TTID Collider         {"Collider"};
    const TTID Material         {"Material"};
    const TTID MeshInstance     {"MeshInstance"};
    const TTID Actor            {"Actor"};
    const TTID PointLight       {"PointLight"};
    const TTID SpotLight        {"SpotLight"};
    const TTID DirectionalLight {"DirectionalLight"};
    const TTID NostalgiaPlayer  {"NostalgiaPlayer"};
}

#endif // THING_TYPES_H
