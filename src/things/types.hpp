#ifndef THING_TYPES_H
#define THING_TYPES_H

#include "ids.hpp"
#include "hash.hpp"

namespace ThingType
{
    constexpr ID Thing            = ConstexprHash("Thing");
    constexpr ID Thinker          = ConstexprHash("Thinker");
    constexpr ID Resource         = ConstexprHash("Resource");
    constexpr ID Mesh             = ConstexprHash("Mesh");
    constexpr ID Texture          = ConstexprHash("Texture");
    constexpr ID Font             = ConstexprHash("Font");
    constexpr ID Device           = ConstexprHash("Device");
    constexpr ID Material         = ConstexprHash("Material");
    constexpr ID MeshInstance     = ConstexprHash("MeshInstance");
    constexpr ID Actor            = ConstexprHash("Actor");
    constexpr ID PointLight       = ConstexprHash("PointLight");
    constexpr ID SpotLight        = ConstexprHash("SpotLight");
    constexpr ID DirectionalLight = ConstexprHash("DirectionalLight");
    constexpr ID NostalgiaPlayer  = ConstexprHash("NostalgiaPlayer");
}

#endif // THING_TYPES_H
