#ifndef THINGS_FWD_H
#define THINGS_FWD_H

class ThingFactory;
struct TTID;

class Thing;

class Thinker;

////////////
// Actors //
////////////
class Actor;
// Cameras
class Camera3D;
class EditorCamera3D;
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
class Collider;
class Material;
class MeshInstance;

///////////////
// Resources //
///////////////
class Resource;
class Mesh;
class Texture;
class Font;

#endif // THINGS_FWD_H
