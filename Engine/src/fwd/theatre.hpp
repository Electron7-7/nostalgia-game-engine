#ifndef THEATRE_FWD_H
#define THEATRE_FWD_H

#include <vector>

class Theatre;

class VariableRegistry;

enum class ThingVarType : unsigned short;

struct ThingVar;
struct ThingData;
struct TheatreParser;
struct TheatreData;

extern unsigned int gColumn,
    gLine,
    gBreakOnColumn,
    gBreakOnLine;

////////////
// THINGS //
////////////
enum class LightType : int;

struct ThingRelative;
class ThingFactory;

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

using relatives_t = std::vector<ThingRelative>;

#endif // THEATRE_FWD_H
