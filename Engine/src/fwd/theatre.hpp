#ifndef THEATRE_FWD_H
#define THEATRE_FWD_H

#include <vector>

class Theatre;

class VariableRegistry;

namespace TheatreFile
{
    enum class ThingVarType;
    enum class TokenName;

    struct Token;
    struct ThingVariable;
    struct ThingData;

    using TokenArray    = std::vector<Token>;
    using ThingVarArray = std::vector<ThingVariable>;
    using TheatreData   = std::vector<ThingData>;
}

////////////
// THINGS //
////////////
class Thing;
    class Resource;
        class Material;
        class Mesh;
        class Texture;
    class Thinker;
    class Viewport;
        class Actor2D;
            class Camera2D;
            class Visual2D;
        class Actor3D;
            class Camera3D;
            class Collider3D;
            class Visual3D;
                class Light3D;
                class PointLight3D;
                class SpotLight3D;
                class DirectionalLight3D;
                class NostalgiaPlayer3D;
                class MeshInstance3D;

#endif // THEATRE_FWD_H
