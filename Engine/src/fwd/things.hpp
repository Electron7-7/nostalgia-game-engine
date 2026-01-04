#ifndef THINGS_FWD_H
#define THINGS_FWD_H

#include <vector>

class ThingFactory;
struct TTID;

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
enum class LightType;
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
