#ifndef THINGS_FWD_H
#define THINGS_FWD_H

#include <vector>

enum class LightType : int;

struct TTID;
struct ThingRelative;

class ThingFactory;
class Thing;
class Thinker;
////////////
// Actors //
////////////
class Actor2D;
class Actor3D;
// Cameras
class Camera3D;
// Lights
class light_t;
class PointLight;
class SpotLight;
class DirectionalLight;
// Player
class NostalgiaPlayer;
/////////////
// Devices //
/////////////
class Device;
class Viewport;
class Collider;
class Material;
class MeshInstance3D;
///////////////
// Resources //
///////////////
class Resource;
class Mesh;
class Texture;
class Font;

using relatives_t = std::vector<ThingRelative>;

#endif // THINGS_FWD_H
