[return to the wiki](/wiki/Home)

_This documentation is heavily inspired by/plagiarizes Godot's [GDScript documentation](https://docs.godotengine.org/en/stable/classes) (specifically, the class reference)._

# Index
- [Thing](thing.md)
    - [Resource](resource.md)
        - [Font](font.md)
        - [Mesh](mesh.md)
        - [Texture](texture.md)
        - [Material](material.md)
    - [Thinker](thinker.md)
        - [Viewport](viewport.md)
        - [Actor3D](actor_3d.md)
            - [NostalgiaPlayer3D](nostalgia_player_3d.md)
            - [Camera3D](camera_3d.md)
            - [Collider3D](collider_3d.md)
            - [Visual3D](visual_3d.md)
                - [MeshInstance3D](mesh_instance_3d.md)
                - [Light3D](light_3d.md)
                    - [PointLight3D](point_light_3d.md)
                    - [SpotLight3D](spot_light_3d.md)
                    - [DirectionalLight3D](directional_light_3d.md)
        - [Actor2D](actor_2d.md)
            - [Camera2D](camera_2d.md)
            - [Visual2D](visual_2d.md)
                - [Sprite2D](sprite_2d.md)
                - [Text2D](text_2d.md)
                - [MeshInstance2D](mesh_instance_2d.md)

----

# Thing

**Inherited By:** [Resource](https://github.com/Electron7-7/nostalgia-game-engine/docs/things/resource.md), [Thinker](https://github.com/Electron7-7/nostalgia-game-engine/docs/things/thinker.md)

The base class for everything else that lives in a [Theatre]().

## Description

Since Nostalgia is partially based on Godot, **Thing** can be compared to Godot's Object class, albeit less advanced. Most, if not all, of the most important virtual functions that your classes will override come from **Thing**. There is an important note to make on overriding virtual functions; when a virtual function is overridden, the original function should be called on the super class at either the beginning or end of the new implementation. For example, if [Thinker]() overrides [Ready()](), it should call `Thing::Ready()` so that it doesn't override potentially important functionality (or if a class that derives from [Thinker]() overrides [Ready()](), it should call `Thinker::Ready()`). This is more important to do for certain functions, especially [SetVariables()]() and [GetVariables()]().

## Functions

| Return Type | Function Signature |
| --- | --- |
| void | [Free](#virtual-void-free)() <sub>virtual</sub>|
| void | [Ready](#virtual-void-ready)() <sub>virtual</sub>|
| void | [Shutdown](#virtual-void-shutdown)() <sub>virtual</sub>|
| void | [Tick](#virtual-void-tick)() <sub>virtual</sub>|
| void | [Update](#virtual-void-update)() <sub>virtual</sub>|
| void | [Input](#virtual-void-inputinputevent)([InputEvent]()\*) <sub>virtual</sub>|
| void | [SetVariables](#virtual-void-setvariablesconst-theatrefilethingdata)(const[TheatreFile::ThingData]()&) <sub>virtual</sub>|
| std::shared_ptr<[TheatreFile::ThingData]()> | virtual [GetVariables](#virtual-stdshared_ptrtheatrefilethingdata-getvariables-const)() const |
| [TheatreFile::ThingData]() | [GetStartingVariables](#virtual-stdshared_ptrtheatrefilethingdata-getvariables-const)() const |
| [ID]() | [uid](#id-uid-const)() const |
| const std::string& | [name](#const-stdstring-name-const)() const |
| void | [set_name](#void-set_namesarg)(Sarg) |
| const char* const | [c_name](#const-char-const-c_name-const)() const |
| const [PID]()& | [type](#const-pid-type-const)() const |

## Function Descriptions
### virtual void Free()
When called, will shutdown and destroy the current **Thing**.

---
### virtual void Ready()
---
### virtual void Shutdown()
---
### virtual void Tick()
---
### virtual void Update()
---
### virtual void Input([InputEvent]()\*)
---
### virtual void SetVariables(const [TheatreFile::ThingData]()&)
---
### virtual std::shared_ptr<[TheatreFile::ThingData]()> GetVariables() const
---
### [TheatreFile::ThingData]() GetStartingVariables() const
---
### [ID]() uid() const
---
### const std::string& name() const
---
### void set_name(Sarg)
---
### const char* const c_name() const
---
### const [PID]()& type() const
----
