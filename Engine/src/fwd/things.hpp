#ifndef THINGS_FWD_H
#define THINGS_FWD_H

#include <vector>

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
        class Actor3D;
            class Camera3D;
            class light_t;
            class PointLight;
            class SpotLight;
            class DirectionalLight;
            class NostalgiaPlayer;
            class MeshInstance3D;
            class Collider;

using relatives_t = std::vector<ThingRelative>;

#endif // THINGS_FWD_H
